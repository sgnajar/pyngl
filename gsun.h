#include <ncarg/hlu/hlu.h>
#include <ncarg/hlu/NresDB.h>
#include <ncarg/hlu/ResList.h>
#include <ncarg/hlu/App.h>
#include <ncarg/hlu/XWorkstation.h>
#include <ncarg/hlu/NcgmWorkstation.h>
#include <ncarg/hlu/PSWorkstation.h>
#include <ncarg/hlu/PDFWorkstation.h>
#include <ncarg/hlu/ContourPlot.h>
#include <ncarg/hlu/XyPlot.h>
#include <ncarg/hlu/MapPlot.h>
#include <ncarg/hlu/VectorPlot.h>
#include <ncarg/hlu/StreamlinePlot.h>
#include <ncarg/hlu/ScalarField.h>
#include <ncarg/hlu/CoordArrays.h>
#include <ncarg/hlu/VectorField.h>
#include <ncarg/hlu/TextItem.h>
#include <ncarg/hlu/GraphicStyle.h>
#include <ncarg/hlu/LogLinPlot.h>
#include <ncarg/hlu/Primitive.h>
#include <netcdf.h>

#define min(x,y)   ((x) < (y) ? (x) : (y))
#define max(x,y)   ((x) > (y) ? (x) : (y))

/*
 * The length of the variable type must be big enough to hold the 
 * longest *numeric* type name, which in this case is "integer".
 */
#define TYPE_LEN 8     

/*
 * Define a structure to hold special resources that aren't
 * set in the normal HLU way (that is, via NhlRLSetxxx).
 */

typedef struct {
  int   gsnMaximize;
  int   gsnDraw;
  int   gsnFrame;
  int   gsnScale;
  int   gsnDebug;

/*
 * Special resources for PS/PDF output.
 */
  int   gsnPaperOrientation;
  float gsnPaperWidth;
  float gsnPaperHeight;
  float gsnPaperMargin;

/*
 * Special resources for paneling.
 */
  int   gsnPanelCenter;
  int   gsnPanelRowSpec;
  float gsnPanelXWhiteSpacePercent;
  float gsnPanelYWhiteSpacePercent;
  int   gsnPanelBoxes;
  float gsnPanelLeft;
  float gsnPanelRight;
  float gsnPanelBottom;
  float gsnPanelTop;
  float gsnPanelInvsblTop;
  float gsnPanelInvsblLeft;
  float gsnPanelInvsblRight;
  float gsnPanelInvsblBottom;
  int   gsnPanelSave;
} gsnRes;

/*
 * Supplemental functions. 
 */

extern int imax_array(int *, int);
extern float xmax_array(float *, int);
extern float xmin_array(float *, int);
extern float *fspan(float, float, int);

extern void compute_ps_device_coords(int, int *, int, gsnRes *);

extern void maximize_plot(int, int *, int, gsnRes *);

extern void scale_plot(int);

extern float *coerce_to_float(void *, const char *, int);

extern int *get_non_missing_pairs(float *, float *, int, int, float *, 
                                  float *, int, int *);

extern void collapse_nomsg(float *, float *, int, int *);

extern void collapse_nomsg_xy(float *, float *, int, int, int, float *,
                              float *, int *);

extern void set_resource(char *, int, void *, const char *, int, int *);

extern int create_graphicstyle_object(int);

extern void draw_and_frame(int, int *, int, gsnRes *);

/*
 * Data object routines.
 */

extern int scalar_field(void *, const char *, int, int, int, void *, 
                        const char *, int, void *, const char *, int,
                        void *, int);

extern int coord_array(void *, void *, const char *, const char *, 
                       int, int *, int, int *, int, int, void *, void *,
                       int);

extern int vector_field(void *, void *, const char *, const char *, int,
                        int, int, void *, const char *, int, void*, 
                        const char *, int, int, void *, void *, int);

/*
 * Workstation routine.
 */

extern int ngl_open_wks_wrap(const char *, const char *, int);

/*
 * Plotting routines.
 */

extern int ngl_contour_wrap(int, void *, const char *, int, int, 
                            int, void *, const char *, int, void *, 
                            const char *, int, void *, int, int, gsnRes *);

extern int ngl_xy_wrap(int, void *, void *, const char *,
                       const char *, int, int *, int, int *, int, int,
                       void *, void *, int, int, int, gsnRes *);

extern int ngl_y_wrap(int, void *, const char *, int, int *, int, void *,
                      int, int, int, gsnRes *);

extern int ngl_vector_wrap(int, void *, void *, const char *, 
                           const char *, int, int, int, void *, 
                           const char *, int, void *, const char *, int,
                           int, void *, void *, int, int, gsnRes *);

extern int ngl_streamline_wrap(int, void *, void *, const char *, 
                               const char *, int, int, int, void *, 
                               const char *, int, void *, const char *, 
                               int, int, void *, void *, int, int, gsnRes *);

extern int ngl_map_wrap(int, int, gsnRes *);

extern int ngl_contour_map_wrap(int, void *, const char *, int, int, 
                                int, void *, const char *, int, void *, 
                                const char *, int, void *, int, int, int,
                                gsnRes *);

extern int ngl_vector_map_wrap(int, void *, void *, const char *, 
                           const char *, int, int, int, void *, 
                           const char *, int, void *, const char *, int,
                           int, void *, void *, int, int, int, gsnRes *);

extern int ngl_streamline_map_wrap(int, void *, void *, const char *, 
                                   const char *, int, int, int, void *, 
                                   const char *, int, void *, const char *, 
                                   int, int, void *, void *, int, int,
                                   int, gsnRes *);

extern int ngl_vector_scalar_wrap(int, void *, void *, void *, const char *, 
                                  const char *, const char *, int, int, int, 
                                  void *, const char *, int, void *, 
                                  const char *, int, int, int, void *, 
                                  void *, void *, int, int, int, gsnRes *);

extern int ngl_vector_scalar_map_wrap(int, void *, void *, void *, 
                                      const char *, const char *, 
                                      const char *, int, int, int, void *,
                                      const char *, int, void *,
                                      const char *, int, int, int, void *, 
                                      void *, void *, int, int, int, int,
                                      gsnRes *);


/*
 * Text routines.
 */

extern int ngl_text_ndc_wrap(int, char *, void *, void *, const char *, 
                             const char *, int, gsnRes *);
extern int ngl_text_wrap(int, int, char *, void *, void *, const char *,
                         const char *, int, gsnRes *);

/*
 * Primitive drawing routines.
 */

extern void ngl_poly_wrap(int, int, void *, void *, const char *type_x,
                          const char *type_y, int, int, int, void *, void*,
                          NhlPolyType, int, int, gsnRes *);


extern int ngl_add_poly_wrap(int, int, void *, void *, const char *, 
                             const char *, int, int, int, void *, void *,
                             NhlPolyType, int, gsnRes *);

extern void ngl_polymarker_ndc_wrap(int, void *, void *, const char *, 
                                    const char *, int, int, int, void *,
                                    void *, int, gsnRes *);

extern void ngl_polyline_ndc_wrap(int, void *, void *, const char *, 
                                  const char *, int, int, int, void *,
                                  void *, int, gsnRes *);

extern void ngl_polygon_ndc_wrap(int, void *, void *, const char *, 
                                 const char *, int, int, int, void *, 
                                 void *, int, gsnRes *);

extern void ngl_polymarker_wrap(int, int, void *, void *, const char *, 
                                const char *, int, int, int, void *, void *, 
                                int, gsnRes *);

extern void ngl_polyline_wrap(int, int, void *, void *, const char *, 
                              const char *, int, int, int, void *, void *, 
                              int, gsnRes *);

extern void ngl_polygon_wrap(int, int, void *, void *, const char *, 
                             const char *, int, int, int, void *, void *, 
                             int, gsnRes *);

extern int ngl_add_polyline_wrap(int, int, void *, void *, const char *, 
                                 const char *, int, int, int, void *, 
                                 void *, int, gsnRes *);

extern int ngl_add_polymarker_wrap(int, int, void *, void *, const char *, 
                                   const char *, int, int, int, void *, 
                                   void *, int, gsnRes *);

extern int ngl_add_polygon_wrap(int, int, void *, void *, const char *, 
                                const char *, int, int, int, void *, 
                                void *, int, gsnRes *);

extern int ngl_add_text_wrap(int, int, char *, void *, void *, const char *, 
                             const char *, int, int, gsnRes *special_res);


extern void ngl_draw_colormap_wrap(int);

extern void ngl_panel_wrap(int, int *, int, int *, int, gsnRes *);