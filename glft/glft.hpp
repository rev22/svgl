#ifndef __glft__
#define __glft__

/*#define __use_gzip__*/
/*#define __use_bzip2__*/
/*#define __use_png__*/

#include <GL/gl.h>

#include <stdio.h>

extern int glft_ft_error;
extern int glft_gl_error;
extern const char* glft_printable;
extern const char* glft_english_printable;

int
glft_init(void);

int
glft_done(void);

int
glft_set_gl_context();

/* ----------------------------------------------------- */
/* font */

struct glft_font;

struct glft_font*
glft_font_new(const char* file);

void
glft_font_delete(struct glft_font* thefont);

int
glft_font_get_ascent(const struct glft_font *);

int
glft_font_get_descent(const struct glft_font *);

int
glft_font_get_line_gap(const struct glft_font *);

/* ----------------------------------------------------- */
/* factory */

struct glft_glyph*
glft_new_glyph(struct glft_font* thefont, char thechar, int size);

struct glft_vector_font*
glft_new_vector_font(struct glft_font*, const char* charset, int size);

struct glft_pixmap_glyph*
glft_new_pixmap_glyph(struct glft_font* thefont, char thechar, int size);

struct glft_pixmap_font*
glft_new_pixmap_font(struct glft_font*, const char* charset, int size);

struct glft_pixmap_font*
glft_new_pixmap_font_maxsize(struct glft_font*, const char* charset, int size, GLint maxsize);

/* ----------------------------------------------------- */
/* glyph */

struct glft_glyph;

void
glft_glyph_delete(struct glft_glyph* theglyph);

struct glft_font*
glft_glyph_get_font(const struct glft_glyph*);

int
glft_glyph_get_xadvance(const struct glft_glyph *);

void
glft_glyph_get_bounding_box(const struct glft_glyph *, int* xmin, int* ymin, int* xmax, int* ymax);

void
glft_glyph_render_plain(struct glft_glyph* theglyph);

void
glft_glyph_render_outline(const struct glft_glyph* theglyph);

int
glft_glyph_save(const struct glft_glyph*, FILE*);

int
glft_glyph_load(struct glft_glyph*, FILE*);

/* ----------------------------------------------------- */
/* vector font */

struct glft_vector_font;

struct glft_vector_font*
glft_vector_font_new();

void
glft_vector_font_delete(struct glft_vector_font *);

void
glft_vector_font_render_plain(const struct glft_vector_font*, const char*);

void
glft_vector_font_render_outline(const struct glft_vector_font*, const char*);

void
glft_vector_font_bounding_box(struct glft_vector_font*, const char*, int* x1, int* y1, int* x2, int* y2);

int
glft_vector_font_get_xadvance(struct glft_vector_font*, const char*);

#if 0
int
glft_vector_font_get_xadvance_char(struct glft_vector_font*, char);
#endif

int
glft_vector_font_save_glyph(const struct glft_vector_font*, char, FILE*);

int
glft_vector_font_load_glyph(struct glft_vector_font*, char, FILE*);

int
glft_vector_font_save(const struct glft_vector_font*, FILE*);

int
glft_vector_font_load(struct glft_vector_font*, FILE*);

/* ----------------------------------------------------- */
/* pixmap glyph */

struct glft_pixmap_glyph;

void
glft_pixmap_glyph_delete(struct glft_pixmap_glyph* theglyph);

struct glft_font*
glft_pixmap_glyph_get_font(const struct glft_glyph*);

int
glft_pixmap_glyph_get_xadvance(const struct glft_pixmap_glyph *);

void
glft_pixmap_glyph_get_bounding_box(const struct glft_pixmap_glyph *, int* xmin, int* ymin, int* xmax, int* ymax);

void
glft_pixmap_glyph_render(struct glft_pixmap_glyph* theglyph);

/* debug */
unsigned char*
glft_pixmap_glyph_get_pixmap(const struct glft_pixmap_glyph *);
/* debug */

/* ----------------------------------------------------- */
/* pixmap font */

struct glft_pixmap_font;

struct glft_pixmap_font*
glft_pixmap_font_new();

void
glft_pixmap_font_delete(struct glft_pixmap_font*);

int
glft_pixmap_font_get_width(const struct glft_pixmap_font*);

int
glft_pixmap_font_get_height(const struct glft_pixmap_font*);

unsigned char*
glft_pixmap_font_get_pixmap(const struct glft_pixmap_font*);

/* debug */
void
glft_pixmap_font_render_texture(const struct glft_pixmap_font*);
/* debug */

void
glft_pixmap_font_enable_texture(const struct glft_pixmap_font*);

void
glft_pixmap_font_render(struct glft_pixmap_font*, const char*);

void
glft_pixmap_font_render_with_other_advance(struct glft_pixmap_font*, struct glft_pixmap_font* otherAdvance, double scale, const char*);

void
glft_pixmap_font_bounding_box(struct glft_pixmap_font*, const char*, int* x1, int* y1, int* x2, int* y2);

void
glft_pixmap_font_bounding_box_with_other_advance(struct glft_pixmap_font*, struct glft_pixmap_font* otherAdvance, double scale, const char*, int* x1, int* y1, int* x2, int* y2);

int
glft_pixmap_font_get_xadvance(struct glft_pixmap_font*, const char*);

#if 0
int
glft_pixmap_font_get_xadvance_char(struct glft_pixmap_font*, char);
#endif

void
glft_pixmap_font_save_pixmap_pgm(const struct glft_pixmap_font*, FILE*);

int
glft_pixmap_font_load_pixmap_pgm(struct glft_pixmap_font*, FILE*);


#ifdef __use_gzip__
void
glft_pixmap_font_save_pixmap_pgm_gzip(const struct glft_pixmap_font*, FILE*);

int
glft_pixmap_font_load_pixmap_pgm_gzip(struct glft_pixmap_font*, FILE*);
#endif // __use_gzip__

#ifdef __use_bzip2__
void
glft_pixmap_font_save_pixmap_pgm_bzip2(const struct glft_pixmap_font*, FILE*);

int
glft_pixmap_font_load_pixmap_pgm_bzip2(struct glft_pixmap_font*, FILE*);
#endif // __use_bzip2__

#ifdef __use_png__
void
glft_pixmap_font_save_pixmap_png(const struct glft_pixmap_font*, FILE*);

int
glft_pixmap_font_load_pixmap_png(struct glft_pixmap_font*, FILE*);
#endif // __use_png__

void
glft_pixmap_font_save_metrics(const struct glft_pixmap_font*, FILE*);

int
glft_pixmap_font_load_metrics(struct glft_pixmap_font*, FILE*);


#endif /* __glft__ */

