/********************************************************************\
 pdfsearch - search and highlight words in pdf, save output only if
 matches were found.
 copyright (c) 2023 till busch.
 
 loosely based on Leela [https://github.com/TrilbyWhite/Leela],
 copyright (c) 2012  Jesse McClure

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <poppler.h>
#include <cairo.h>
#include <cairo-pdf.h>

static void pdfsearch_help();
static void pdfsearch_search(PopplerDocument *, const gchar* searchstring, const gchar* outfile);
static PopplerDocument *open_pdf(const char *);

void pdfsearch_search(PopplerDocument *pdf, const gchar* searchstring, const gchar* outfile)
{
	PopplerPage *page;
	PopplerAnnot *annot;

	GError *gerror = NULL;
	GArray *garray = NULL;
	GList *annots, *list, *rects;
	gchar *uri;
	gchar **words = g_strsplit(searchstring, "|", -1);
	// double width, height;
	int i, j, w;
	int matches = 0;
	int npages = poppler_document_get_n_pages(pdf);
	for (i = 0; i < npages; i++)
	{
		page = poppler_document_get_page(pdf, i);
		w = 0;
		gchar *word = words[w];
		while (word)
		{
			// poppler_page_get_size (page, &width, &height);
			// POPPLER_FIND_IGNORE_DIACRITICS // requires pure ascii input
			rects = poppler_page_find_text_with_options(page, word, POPPLER_FIND_MULTILINE);
			for (list = rects, j = 0; list; list = list->next, j++)
			{
				matches++;
				PopplerRectangle *r = list->data;
				PopplerQuadrilateral q;
				garray = g_array_new(FALSE, FALSE, sizeof(PopplerQuadrilateral));
				q.p1.x = r->x1;
				q.p1.y = r->y2;
				q.p2.x = r->x2;
				q.p2.y = r->y2;
				q.p3.x = r->x1;
				q.p3.y = r->y1;
				q.p4.x = r->x2;
				q.p4.y = r->y1;
				g_array_append_val(garray, q);
				fprintf(stderr, "    match '%s' at %0.2f %0.2f %0.2f %0.2f\n", word, r->x1, r->y1, r->x2, r->y2);
				annot = poppler_annot_text_markup_new_highlight(pdf, r, garray);

				PopplerColor pc;
				pc.red = 0xffff;
				pc.green = 0xffff;
				pc.blue =  0x4444;
				poppler_annot_set_color(annot, &pc);
				poppler_annot_set_contents(annot, word);
				poppler_page_add_annot(page, annot);

				g_array_unref(garray);
				g_object_unref(annot);
			}
			g_list_free_full(rects, free);
			word = words[++w];
		}
		g_object_unref(page);
	}
	g_strfreev(words);
	if(matches) {
		gchar *path = g_path_get_dirname(outfile);
		// fprintf(stderr, "path='%s'\n", path);
		gchar *basename = g_path_get_basename(outfile);
		// fprintf(stderr, "basename='%s'\n", basename);
		mkdir(path, 0777);
		gchar *rpath = realpath(path, NULL);
		// fprintf(stderr, "rpath='%s'\n", rpath);
		gchar *abspath = g_build_filename(rpath, basename, NULL);
		// fprintf(stderr, "abspath='%s'\n", rpath);
		uri = g_filename_to_uri(abspath, NULL, &gerror);
		// fprintf(stderr, "uri='%s'\n", uri);
		poppler_document_save(pdf, uri, &gerror);
		g_free(uri);
		g_free(abspath);
		g_free(rpath);
		g_free(basename);
		g_free(path);
	}
}

void pdfsearch_help()
{
	int i;
	printf(
		"pdfsearch v0.1 copyright (c) 2023 by till busch.\n"
		"loosely based on\n"
		"Leela v0.3 Copyright (C) 2012-2013  Jesse McClure.\n"
		"This program comes with ABSOLUTELY NO WARRANTY\n"
		"This is free software, and you are welcome to redistribute\n"
		"it under the conditions of the GPLv3 license.\n\n"
		"USAGE: pdfsearch <pdf> <search string> <output>\n"
		"  pdf            filename of input pdf\n"
		"  search string  words to search and highlight (separated by |)\n"
		"  output         output filename\n");
}

PopplerDocument *open_pdf(const char *arg)
{
	char *fullpath = realpath(arg, NULL);
	// fprintf(stderr, "filename=%s\n", g_path_get_basename(fullpath));
	if (!fullpath)
	{
		fprintf(stderr, "Cannot find file \"%s\"\n", arg);
		exit(1);
	}
	char *uri = (char *)malloc(strlen(fullpath) + 8);
	strcpy(uri, "file://");
	strcat(uri, fullpath);
	free(fullpath);
	PopplerDocument *pdf = poppler_document_new_from_file(uri, NULL, NULL);
	if(!pdf)
		fprintf(stderr, "Failed to open document.\n");
	free(uri);
	return pdf;
}

int main(int argc, const char **argv)
{
	int i = 1;
	if(argc < 4) {
		pdfsearch_help();
		return 1;
	}
	const gchar *filename = argv[i++];
	const gchar *searchstring = argv[i++];
	const gchar *outfile = argv[i++];
	PopplerDocument *pdf = open_pdf(filename);
	pdfsearch_search(pdf, searchstring, outfile);
	g_object_unref(pdf);
	return 0;
}
