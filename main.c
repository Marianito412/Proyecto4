#include <gtk/gtk.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "backtracking.c"

enum Variant {
  BASIC,
  RANGE,
  GE,
  GEL
}; 

enum Variant currentVariant = BASIC;

GtkFlowBox* valueBox = NULL;
GtkBox* resultBox = NULL;
GtkBox* rangeBox = NULL;
GtkSpinButton* spinRange = NULL;
GtkScrolledWindow* scrollBox = NULL;
GtkLabel* resultsLabel = NULL;

GtkDrawingArea* InputArea = NULL;
GtkLabel* InputLabel = NULL;

void UpdateModel(){
    int Total = 0;

    for(int i=0; i<n; i++){
        printf("%d", w[i]);
        Total+=w[i];
    }
    char Model[100];
    sprintf(Model, "K(%d;", Total);
    
    for(int i=0; i<n; i++){
        char voter[10];
        if (i >= n-1){
            sprintf(voter, " %d", w[i]);
        }
        else{
            sprintf(voter, " %d,", w[i]);
        }
        strcat(Model, voter);
    }
    strcat(Model, ")");

    gtk_label_set_text(InputLabel, Model);
}

GtkBox* generate_row(){
    GtkBox* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    int counter = 0;
    for (int i = 0; i<n; i++){
        GtkCheckButton* button = gtk_check_button_new();
        char name[100];
        sprintf(name, "a%d: %d", i, w[i]);
        GtkLabel* label = gtk_label_new(name);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), include[i]);
        //printf("%d, ", include[i]);
        gtk_box_pack_start(box, label, true, true, 2);
        gtk_box_pack_start(box, button, true, true, 2);

        if (include[i]){
            counter += w[i];
        }
    }

    char total[100];
    sprintf(total, "Total: %d", counter);
    GtkLabel* totalLabel = gtk_label_new(total);
    gtk_box_pack_start(box, totalLabel, true, true, 2);
    //printf("\n");
    
    return box;
}

void writeSolution(){
    //printf("NewSolution\n");
    GtkBox* NewSolution = generate_row();
    gtk_box_pack_start(resultBox, NewSolution, true, true, 2);
    //gtk_flow_box_insert(resultBox, NewSolution, -1);
    gtk_widget_show_all(resultBox);
}

void LoadNumbers(){
    int i = 0;
    memset(w, 0, sizeof(int) * n);

    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(valueBox));
    for(iter = children; iter != NULL; iter = g_list_next(iter)){
        GtkFlowBoxChild* child = GTK_FLOW_BOX_CHILD(iter->data);
        GtkSpinButton* Button = GTK_SPIN_BUTTON(gtk_bin_get_child(child));
        int num = gtk_spin_button_get_value_as_int(Button);
        w[i] = num;
        i++;
    }
    g_list_free(children);

    /*
    for (int i=0; i<n; i++){
        printf("%d, ", w[i]);
    }
    printf("\n");
    */
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

int Test() {  // SE PUEDE ELIMINAR ESTO, SOLO ES PARA PRUEBAS
    int option;
    int total;
    
    printf("Ingrese la cantidad de numeros: ");
    scanf("%d", &n);
    
    printf("Ingrese los %d numeros:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &w[i]);
    }
    
    sort_descending();
    
    printf("Ingrese W: ");
    scanf("%d", &W);
    
    total = 0;
    for (int i = 0; i < n; i++) {
        total += w[i];
    }
    
    while (1) {
        printf("\nMenu:\n");
        printf("1. Suma exacta W\n");
        printf("2. Suma en rango [W-Δ, W+Δ]\n");
        printf("3. Suma mayor o igual a W (sin acotar)\n");
        printf("4. Suma mayor o igual a W (acotada)\n");
        printf("5. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &option);
        
        if (option == 5) break;
        
        // Reiniciar contadores
        total_nodes = 0;
        solution_count = 0;
        memset(include, 0, sizeof(int) * n);
        
        switch (option) {
            case 1:
                printf("\nSoluciones con suma exacta %d:\n", W);
                subset_sum_exact(0, 0, total);
                break;
                
            case 2:
                printf("Ingrese Δ (delta): ");
                scanf("%d", &delta);
                printf("\nSoluciones con suma en [%d, %d]:\n", W-delta, W+delta);
                subset_sum_range(0, 0, total);
                break;
                
            case 3:
                printf("\nSoluciones con suma >= %d:\n", W);
                subset_sum_ge(0, 0, total);
                break;
                
            case 4:
                printf("\nSoluciones con suma >= %d (acotadas):\n", W);
                subset_sum_ge_bounded(0, 0, total);
                break;
                
            default:
                printf("Opcion no valida.\n");
        }
        
        printf("\nTotal de soluciones encontradas: %d\n", solution_count);
        printf("Total de nodos visitados: %d\n", total_nodes);
    }
    
    return 0;
}

void OnSetChanged(GtkSpinButton* Button, gpointer UserData){
    LoadNumbers();
    ClampRange();
    UpdateModel();

    int value = gtk_spin_button_get_value_as_int(spinRange);
    delta = value;
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
        gtk_flow_box_insert(valueBox, newButton, 0);
    }

    n = value;

    gtk_widget_show_all(valueBox);
}

void OnWChanged(GtkSpinButton* Button, gpointer UserData){
    int value = gtk_spin_button_get_value_as_int(Button);   
    W = value;
}

void OnRangeChanged(GtkSpinButton* Button, gpointer UserData){
    ClampRange();
    int value = gtk_spin_button_get_value_as_int(Button);
    delta = value;
    //printf("NewRange: %d\n", delta);
}

void OnVariantChanged(GtkRadioButton* Button, gpointer UserData){
    bool Test = gtk_toggle_button_get_active(Button);
    if (!Test) return;

    if ((int)UserData == 1){
        gtk_widget_set_visible(rangeBox, true);
    }else{
        gtk_widget_set_visible(rangeBox, false);
    }
    currentVariant = (enum Variant)UserData;
    //printf("%s - %d \n", gtk_button_get_label(Button), currentVariant);
}

void onBeginExecute(GtkButton* Button, gpointer UserData){
    //printf("---------------StartNewSolution-----------------");
    LoadNumbers();

    int total = 0;
    for (int i = 0; i < n; i++) {
        total += w[i];
    }

    GList *children, *iter;

    children = gtk_container_get_children(GTK_CONTAINER(resultBox));
    for(iter = children; iter != NULL; iter = g_list_next(iter))
      gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    solution_count = 0;
    total_nodes = 0;

    switch (currentVariant)
    {
    case BASIC:
        /* code */
        subset_sum_exact(0, 0, total);
        break;
    case RANGE:
        /* code */
        subset_sum_range(0, 0, total);
        break;
    case GE:
        /* code */
        subset_sum_ge(0, 0, total);
        break;
    case GEL:
        /* code */
        subset_sum_ge_bounded(0, 0, total);
        break;
    default:
        break;
    }

    gtk_widget_show_all(scrollBox);
    char results[100];
    sprintf(results, "Solutions found: %d | Nodes visited: %d", solution_count, total_nodes);
    gtk_label_set_text(resultsLabel, results);
    gtk_widget_show_all(resultsLabel);

}

static gboolean drawInput(GtkWidget *widget, cairo_t *cr) {

    GdkRGBA test[] = {
        {0.0, 0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {0.0, 1.0, 1.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 1.0, 1.0},
        {1.0, 1.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0},
        {1.0, 1.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0},
        {1.0, 1.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0}
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
    cairo_set_source_rgb(cr, 1, 1, 1);  // Fondo negro (puedes cambiarlo)
    cairo_paint(cr);  // Llena toda la superficie con el color de fondo

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

        cairo_set_line_width (cr, 30.0);
        cairo_set_source_rgb(cr, test[i].red, test[i].green, test[i].blue);
        cairo_stroke(cr);
    }
    printf("Test\n");

    return 0;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    gtk_init(&argc, &argv);
    reportSolution = &writeSolution;

    GtkBuilder* builder = gtk_builder_new_from_file("interface.glade");

    // Ventana principal
    GtkWidget* window = GTK_WIDGET(gtk_builder_get_object(builder, "Window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    //Miembros del conjunto
    valueBox = GTK_FLOW_BOX(gtk_builder_get_object(builder, "value_box"));

    //Tamaño del conjunto
    GtkSpinButton* spinN = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_n"));
    g_signal_connect(spinN, "value-changed", G_CALLBACK(OnNChanged), NULL);

    //Número objetivo (W)
    GtkSpinButton* spinW = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spin_W"));
    g_signal_connect(spinW, "value-changed", G_CALLBACK(OnWChanged), NULL);

    //Opciones de variantes
    /*
    GtkRadioButton* radioBasic = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radio_basic"));
    g_signal_connect(radioBasic, "toggled", G_CALLBACK(OnVariantChanged), BASIC);

    GtkRadioButton* radioRange = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radio_range"));
    g_signal_connect(radioRange, "toggled", G_CALLBACK(OnVariantChanged), RANGE);

    GtkRadioButton* radioGE = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radio_ge"));
    g_signal_connect(radioGE, "toggled", G_CALLBACK(OnVariantChanged), GE);

    GtkRadioButton* radioGEL = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radio_ge_limited"));
    g_signal_connect(radioGEL, "toggled", G_CALLBACK(OnVariantChanged), GEL);
    */

   //InputModelDrawing
   InputArea = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "InputArea"));
   g_signal_connect(G_OBJECT(InputArea), "draw", G_CALLBACK(drawInput), NULL);

   //InputModelLabel
    InputLabel = GTK_LABEL(gtk_builder_get_object(builder, "BPIModel"));

    //Rango de resultado
    rangeBox = GTK_BOX(gtk_builder_get_object(builder, "RangeBox"));
    spinRange = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "Range"));
    g_signal_connect(spinRange, "value-changed", G_CALLBACK(OnRangeChanged), NULL);

    //Boton de Ejecutar
    GtkButton* buttonExecute = GTK_BUTTON(gtk_builder_get_object(builder, "btn_execute"));
    g_signal_connect(buttonExecute, "pressed", G_CALLBACK(onBeginExecute), NULL);

    //Resultados
    resultBox = GTK_BOX(gtk_builder_get_object(builder, "ResultBox"));
    resultsLabel = GTK_LABEL(gtk_builder_get_object(builder, "lbl_results"));

    //scrollBox
    scrollBox = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "scrollBox"));

    OnNChanged(spinN, NULL);

    gtk_widget_show_all(window);
    gtk_widget_set_visible(rangeBox, false);
    gtk_widget_show_all(valueBox);
    gtk_widget_queue_draw(GTK_WIDGET(InputArea));
    gtk_main();
    return 0;
}