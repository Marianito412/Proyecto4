#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "backtracking.c"

#define M_PI 3.14159265358979323846 

// Variables globales
GtkFlowBox* valueBox = NULL;
GtkBox* resultBox = NULL;
GtkBox* rangeBox = NULL;
GtkSpinButton* spinRange = NULL;
GtkScrolledWindow* scrollBox = NULL;
GtkLabel* resultsLabel = NULL;
GtkDrawingArea* InputArea = NULL;
GtkLabel* InputLabel = NULL;

// Prototipos de funciones
void UpdateModel();
void LoadNumbers();
void show_coalitions_text(GtkBox *container);
void show_coalitions_graphic(GtkBox *container, int total_votes);
static gboolean on_draw_coalition(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static void setup_ipb_labels(GtkWidget *da, GdkRectangle *allocation, gpointer user_data);


void UpdateModel() {
    int Total = 0;
    for(int i = 0; i < n; i++) {
        Total += w[i];
    }
    
    char Model[100];
    sprintf(Model, "Modelo: (%d;", W);
    
    for(int i = 0; i < n; i++) {
        char voter[10];
        sprintf(voter, " %d%c", w[i], (i == n-1) ? ')' : ',');
        strcat(Model, voter);
    }

    gtk_label_set_text(InputLabel, Model);
}

void LoadNumbers() {
    int i = 0;
    memset(w, 0, sizeof(int) * n);

    GList *children = gtk_container_get_children(GTK_CONTAINER(valueBox));
    for(GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        GtkSpinButton* Button = GTK_SPIN_BUTTON(gtk_bin_get_child(GTK_BIN(iter->data)));
        w[i++] = gtk_spin_button_get_value_as_int(Button);
    }
    g_list_free(children);
}

void ClampRange(){
    LoadNumbers();
    int min = INT_MAX;
    for(int i=0; i<n; i++){
        if (w[i]<min){
            min = w[i];
        }
    }
    int value = gtk_spin_button_get_value_as_int(spinRange);
    if (value>min){
        gtk_spin_button_set_value(spinRange, min);
    }
}

void OnSetChanged(GtkSpinButton* Button, gpointer UserData){
    LoadNumbers();
    ClampRange();
    UpdateModel();

    int value = gtk_spin_button_get_value_as_int(spinRange);
    gtk_widget_queue_draw(GTK_WIDGET(InputArea));
    //printf("NewRange: %d\n", delta);
}

void OnNChanged(GtkSpinButton* Button, gpointer UserData){
    int value = gtk_spin_button_get_value_as_int(Button);

    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(valueBox));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
      gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    for(int i = 0; i < value; i++){
        GtkAdjustment* adjustment = gtk_adjustment_new (1.0, 0.0, 1000000000000000000, 1.0, 5.0, 0.0);
        GtkSpinButton* newButton = gtk_spin_button_new(adjustment, 1, 0);
        g_signal_connect(newButton, "value-changed", G_CALLBACK(OnSetChanged), NULL);
        OnSetChanged(newButton, NULL);
        gtk_flow_box_insert(valueBox, newButton, 0);
    }

    n = value;

    gtk_widget_show_all(valueBox);
}

void OnWChanged(GtkSpinButton* Button, gpointer UserData) {
    (void)UserData;
    W = gtk_spin_button_get_value_as_int(Button);
    UpdateModel();
}

static gboolean drawInput(GtkWidget *widget, cairo_t *cr) {

    GdkRGBA test[] = {
        {0.16, 0.49, 0.82, 1.0},  // 1. Azul vibrante
        {0.89, 0.29, 0.30, 1.0},  // 2. Rojo intenso
        {0.30, 0.69, 0.31, 1.0},  // 3. Verde esmeralda
        {0.49, 0.35, 0.09, 1.0},  // 4. Cobre oscuro 
        {0.61, 0.31, 0.71, 1.0},  // 5. Púrpura real
        {0.09, 0.75, 0.81, 1.0},  // 6. Turquesa brillante
        {0.95, 0.52, 0.13, 1.0},  // 7. Naranja fuego
        {0.46, 0.33, 0.64, 1.0},  // 8. Índigo profundo
        {0.80, 0.18, 0.40, 1.0},  // 9. Rosa fucsia
        {0.00, 0.62, 0.45, 1.0},  // 10. Verde océano
        {0.93, 0.46, 0.65, 1.0},  // 11. Rosa claro
        {0.70, 0.47, 0.20, 1.0}   // 12. Marrón dorado
    };

    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);
    int Total = 0;

    LoadNumbers();
    
    for(int i=0; i<n; i++){
        printf("%d ", w[i]);
        Total+=w[i];
    }

    printf(" %d \n", Total);
    //cairo_set_source_rgb(cr, 0, 0, 0);  // Fondo negro 
    //cairo_paint(cr);  // Llena toda la superficie con el color de fondo

    cairo_move_to(cr,0,0);
    float currentWidth = 0;
    for(int i=0; i<n; i++){
        float prop = ((float)w[i]/(float)Total);
        printf("Value: %d ", w[i]);
        printf("Prop: %f ", prop);
        printf("Size: %f ", prop*width);
        cairo_move_to(cr, currentWidth, 0);
        currentWidth += prop*width;
        cairo_line_to(cr, currentWidth, 0);

        cairo_set_line_width (cr, 100);
        cairo_set_source_rgb(cr, test[i].red, test[i].green, test[i].blue);
        cairo_stroke(cr);
    }
    printf("Test\n");

    return 0;
}

void onBeginExecute(GtkButton* Button, gpointer UserData) {
    LoadNumbers();
    
    // Validar entrada
    if (W <= 0) {
        gtk_label_set_text(resultsLabel, "Error: K debe ser mayor que 0");
        return;
    }

    // Limpiar resultados anteriores
    GList *children = gtk_container_get_children(GTK_CONTAINER(resultBox));
    for(GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Ejecutar algoritmo
    banzhaf(n, W, w);
    
    // Calcular total de votos
    int total_votes = 0;
    for (int i = 0; i < n; i++) {
        total_votes += w[i];
    }
    
    // Mostrar resultados
    //show_coalitions_text(resultBox);
    show_coalitions_graphic(resultBox, total_votes);
    
    // Mostrar estadísticas
    int total_critical = 0;
    int* critical_votes = malloc(n * sizeof(int));
    banzhaf_get_critical_votes(critical_votes);

    for (int i = 0; i < n; i++) {
        total_critical += critical_votes[i];
    }
    free(critical_votes);

    char results[150];
    sprintf(results, "Coaliciones encontradas: %d | Total votos críticos: %d", banzhaf_get_solution_count(), total_critical);
    gtk_label_set_text(resultsLabel, results);

    gtk_widget_show_all(scrollBox);
}

void show_coalitions_graphic(GtkBox *container, int total_votes) {
    // Limpiar el contenedor primero
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    int num_coaliciones;
    Coalicion *coaliciones = banzhaf_get_coalitions(&num_coaliciones);
    int critical_votes[MAX_N];
    double power_index[MAX_N];
    
    // Obtener votos críticos y calcular IPB
    banzhaf_get_critical_votes(critical_votes);
    int total_critical = 0;
    for (int i = 0; i < n; i++) {
        total_critical += critical_votes[i];
    }
    for (int i = 0; i < n; i++) {
        power_index[i] = total_critical > 0 ? 
                       (double)critical_votes[i] / total_critical : 0.0;
    }

    for (int i = 0; i < num_coaliciones; i++) {
        // Crear contenedor vertical para cada coalición
        GtkWidget *coalitionBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        
        // Área de dibujo
        GtkWidget *da = gtk_drawing_area_new();
        gtk_widget_set_hexpand(da, TRUE);
        gtk_widget_set_size_request(da, -1, 40);  // Altura fija de 40px
        
        g_object_set_data(G_OBJECT(da), "coalicion", &coaliciones[i]);
        g_object_set_data(G_OBJECT(da), "total_votos", GINT_TO_POINTER(total_votes));
        
        g_signal_connect(da, "draw", G_CALLBACK(on_draw_coalition), NULL);
        gtk_box_pack_start(GTK_BOX(coalitionBox), da, FALSE, FALSE, 0);
        
        // Contenedor para IPBs
        GtkWidget *ipbContainer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0); // Sin espaciado
        gtk_widget_set_margin_top(ipbContainer, 5);
        gtk_widget_set_margin_bottom(ipbContainer, 5);
        
        // Conectar señal para configurar los IPBs después de que se asigne el tamaño
        g_signal_connect(da, "size-allocate", G_CALLBACK(setup_ipb_labels), ipbContainer);
        g_object_set_data(G_OBJECT(da), "ipb_container", ipbContainer);
        g_object_set_data(G_OBJECT(da), "total_votes", GINT_TO_POINTER(total_votes));
        
        gtk_box_pack_start(GTK_BOX(coalitionBox), ipbContainer, FALSE, FALSE, 0);
        gtk_box_pack_start(container, coalitionBox, FALSE, FALSE, 5);
    }
    
    gtk_widget_show_all(container);
}

// Nueva función para configurar las etiquetas IPB después de conocer el tamaño
static void setup_ipb_labels(GtkWidget *da, GdkRectangle *allocation, gpointer user_data) {
    GtkWidget *ipbContainer = GTK_WIDGET(user_data);
    Coalicion *c = (Coalicion *)g_object_get_data(G_OBJECT(da), "coalicion");
    int total_votes = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(da), "total_votes"));
    
    // Obtener datos de votos críticos y calcular IPB
    int critical_votes[MAX_N];
    banzhaf_get_critical_votes(critical_votes);
    int total_critical = 0;
    for (int i = 0; i < n; i++) {
        total_critical += critical_votes[i];
    }
    
    double power_index[MAX_N];
    for (int i = 0; i < n; i++) {
        power_index[i] = total_critical > 0 ? 
                       (double)critical_votes[i] / total_critical : 0.0;
    }

    // Limpiar el contenedor primero
    GList *children = gtk_container_get_children(GTK_CONTAINER(ipbContainer));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    // Obtener el ancho disponible
    int available_width = allocation->width;
    double votes_per_pixel = (double)total_votes / available_width;

    // Crear etiquetas para cada votante
    for (int j = 0; j < c->size; j++) {
        if (c->votos[j] > 0) {
            // Calcular ancho en píxeles
            int pixel_width = (int)(c->votos[j] / votes_per_pixel);
            pixel_width = MAX(pixel_width, 1);

            GtkWidget *frame = gtk_frame_new(NULL);
            gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
            gtk_widget_set_size_request(frame, pixel_width, -1);

            GtkWidget *label = gtk_label_new(NULL);
            gtk_label_set_xalign(GTK_LABEL(label), 0.5);
            
            if (pixel_width > 50) {
                char ipb_text[50];
                snprintf(ipb_text, sizeof(ipb_text), "%.4f\n(%d/%d)", 
                        power_index[j], critical_votes[j], total_critical);
                gtk_label_set_text(GTK_LABEL(label), ipb_text);
            }

            // Configurar estilo
            GtkCssProvider *provider = gtk_css_provider_new();
            gtk_css_provider_load_from_data(provider,
                "frame {"
                "   background: transparent;"
                "   border: none;"
                "   margin: 0;"
                "   padding: 0;"
                "}"
                ""
                "label {"
                "   font-family: 'Segoe UI', Roboto, sans-serif;"
                "   font-size: 11px;"
                "   font-weight: bold;"
                "   color: white;"
                "   padding: 2px 4px;"
                "   text-shadow: 1px 1px 2px rgba(0,0,0,0.5);"
                "}", -1, NULL);
            
            GtkStyleContext *context = gtk_widget_get_style_context(label);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), 
                                         GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

            gtk_container_add(GTK_CONTAINER(frame), label);
            gtk_box_pack_start(GTK_BOX(ipbContainer), frame, FALSE, FALSE, 0);
        }
    }
    
    gtk_widget_show_all(ipbContainer);
}

static gboolean on_draw_coalition(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    Coalicion *c = (Coalicion *)g_object_get_data(G_OBJECT(widget), "coalicion");
    int total_votes = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "total_votos"));

    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);

    // Colores 
    GdkRGBA colors[] = {
        {0.16, 0.49, 0.82, 1.0},  // 1. Azul vibrante
        {0.89, 0.29, 0.30, 1.0},  // 2. Rojo intenso
        {0.30, 0.69, 0.31, 1.0},  // 3. Verde esmeralda
        {0.49, 0.35, 0.09, 1.0},  // 4. Cobre oscuro 
        {0.61, 0.31, 0.71, 1.0},  // 5. Púrpura real
        {0.09, 0.75, 0.81, 1.0},  // 6. Turquesa brillante
        {0.95, 0.52, 0.13, 1.0},  // 7. Naranja fuego
        {0.46, 0.33, 0.64, 1.0},  // 8. Índigo profundo
        {0.80, 0.18, 0.40, 1.0},  // 9. Rosa fucsia
        {0.00, 0.62, 0.45, 1.0},  // 10. Verde océano
        {0.93, 0.46, 0.65, 1.0},  // 11. Rosa claro
        {0.70, 0.47, 0.20, 1.0}   // 12. Marrón dorado
    };

    // Dibujar los segmentos de votos
    double current_x = 0;
    for (int i = 0; i < c->size; i++) {
        if (c->votos[i] > 0) {
            double segment_width = (double)width * c->votos[i] / total_votes;
            
            // Dibujar rectángulo relleno
            cairo_rectangle(cr, current_x, 0, segment_width, height);
            cairo_set_source_rgb(cr, colors[i%6].red, colors[i%6].green, colors[i%6].blue);
            cairo_fill_preserve(cr);
            
            // Dibujar borde negro
            cairo_set_source_rgb(cr, 0, 0, 0);
            cairo_set_line_width(cr, 1.0);
            cairo_stroke(cr);

            // Marcar votante crítico con estrella dorada
            if (c->criticos[i]) {
                cairo_save(cr);
                cairo_set_source_rgb(cr, 1.0, 0.84, 0.0);  // Dorado
                
                double star_size = fmin(segment_width, height) * 0.15;
                double center_x = current_x + segment_width/2;
                double center_y = height/2;
                
                // Dibujar estrella de 5 puntas
                for (int j = 0; j < 5; j++) {
                    double angle = 2.0 * M_PI * j / 5.0 - M_PI/2.0;
                    double x = center_x + cos(angle) * star_size;
                    double y = center_y + sin(angle) * star_size;
                    
                    if (j == 0) cairo_move_to(cr, x, y);
                    else cairo_line_to(cr, x, y);
                    
                    angle += 2.0 * M_PI / 10.0;
                    x = center_x + cos(angle) * (star_size * 0.4);
                    y = center_y + sin(angle) * (star_size * 0.4);
                    cairo_line_to(cr, x, y);
                }
                cairo_close_path(cr);
                cairo_fill(cr);
                cairo_restore(cr);
            }
            
            current_x += segment_width;
        }
    }

    return FALSE;
}

    // Mostrar estadísticas en la parte inferior
    /*
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 10);
    
    char stats[100];
    snprintf(stats, sizeof(stats), "Votos críticos totales: %d", total_criticos);
    cairo_move_to(cr, 5, height - 5);
    cairo_show_text(cr, stats);
    */



int main(int argc, char *argv[]) {
    // Inicialización
    gtk_init(&argc, &argv);
    
    // Cargar interfaz desde Glade
    GtkBuilder* builder = gtk_builder_new_from_file("interface.glade");
    if (!builder) {
        g_error("Error al cargar el archivo interface.glade");
        return 1;
    }

    // Obtener widgets de la interfaz
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder, "Window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Configurar widgets importantes
    valueBox = GTK_FLOW_BOX(gtk_builder_get_object(builder, "value_box"));
    resultBox = GTK_BOX(gtk_builder_get_object(builder, "ResultBox"));
    scrollBox = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "scrollBox"));
    resultsLabel = GTK_LABEL(gtk_builder_get_object(builder, "lbl_results"));
    InputArea = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "InputArea"));
    InputLabel = GTK_LABEL(gtk_builder_get_object(builder, "BPIModel"));

    // Configurar spin buttons
    GtkSpinButton* spinN = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_n"));
    GtkSpinButton* spinW = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_W"));
    spinRange = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "Range"));
    rangeBox = GTK_BOX(gtk_builder_get_object(builder, "RangeBox"));

    // Conectar señales
    g_signal_connect(spinN, "value-changed", G_CALLBACK(OnNChanged), NULL);
    g_signal_connect(spinW, "value-changed", G_CALLBACK(OnWChanged), NULL);
    //g_signal_connect(spinRange, "value-changed", G_CALLBACK(OnRangeChanged), NULL);
    g_signal_connect(InputArea, "draw", G_CALLBACK(drawInput), NULL);

    // Configurar botón de ejecución
    GtkButton* buttonExecute = GTK_BUTTON(gtk_builder_get_object(builder, "btn_execute"));
    g_signal_connect(buttonExecute, "clicked", G_CALLBACK(onBeginExecute), NULL);

    // Inicializar interfaz
    OnNChanged(spinN, NULL);  // Configura los spin buttons iniciales
    OnWChanged(spinW, NULL);  // Configura los spin buttons iniciales
    gtk_widget_set_visible(rangeBox, FALSE);  // Ocultar rango (no usado en este proyecto)
    
    // Mostrar ventana principal
    gtk_widget_show_all(window);
    gtk_widget_queue_draw(InputArea);  // Forzar redibujado del área de entrada
    // Liberar builder después de usar
    g_object_unref(builder);

    // Bucle principal de GTK
    gtk_main();

    return 0;
}