#include "sqlite3.h"

#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    gchar *eng;
    gchar *vie;
}Dict;

static Dict data[100];
static int cur=0;
static int curTemp=0;
char tmp[100];
char eng[40], vie[40], sqlStm[3000];


// main window
GtkBuilder *builder;
GtkWidget *window;
GtkWidget *scroll1;
GtkWidget *fixed1;
GtkWidget *search;
GtkWidget *deleteBtn;
GtkWidget *confirmBtn;
GtkWidget *cancelBtn;
GtkWidget *box;
GtkTreeSelection *selection;
GtkTreeView *mybookview;
GtkWidget *titleLabel;

// add window
GtkWidget *addWindow;
GtkWidget *notiLabel1;
GtkWidget *fixed2;
GtkWidget *scroll3;
GtkWidget *engEntry3;
GtkWidget *addBtn1;
GtkWidget *viewport1;
GtkWidget *textview1;
GtkTextBuffer *TextBuffer1;
GtkWidget *addPop;

// edit window
GtkWidget *editWindow;
GtkWidget *notiLabel3;
GtkWidget *engEntry1;
GtkWidget *fixed3;
GtkWidget *scroll2;
GtkWidget *viewport2;
GtkWidget *saveBtn;
GtkWidget *textview2;
GtkTextBuffer *TextBuffer2;
GtkWidget *editPop;

GtkWidget *viewPop;
GtkWidget *favePop;

// Man hinh thong bao
GtkWidget *messageDialog;
GtkWidget *messageBox;
GtkWidget *massageBtn1;
GtkWidget *massageBtn2;
GtkWidget *messageLabel;

// Man hinh thong bao xoa thanh cong
GtkWidget *checkWindow;
GtkWidget *checkFixed;
GtkWidget *checkLabel;
GtkWidget *checkImage;
GtkWidget *checkBtn;

//LICH SU
GtkWidget *historyPop;

GtkListStore *history;
GtkTreeView *historyview;
GtkWidget *historyWindow;
GtkWidget *fixed4;
GtkWidget *historylabel;
GtkWidget *historyscroll;
GtkTreeIter hisiter;
int hiscount = 0;

//Favorite
GtkListStore *fave;
GtkTreeView *faveview;
GtkWidget *faveWindow;
GtkWidget *favefixed;
GtkWidget *favelabel;
GtkWidget *favesearch;
GtkWidget *favescroll;
GtkWidget *flashcardPop;
int faveCount = 0;

//Flashcard
GtkWidget *flashWindow;
GtkWidget *flashBtn;
GtkWidget *flashfixed;
GtkWidget *nextBtn;
GtkTreeIter ansIter;
int anscol = 0;


static void onRowActivated (GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data);
int findRow(GtkTreeModel *model, GtkTreeIter iter, const gchar *srch);
void on_changed_text1(GtkTextBuffer *t);
void on_changed_text2(GtkTextBuffer *t);
void on_addWindow_destroy();
void on_editWindow_destroy();
void remove_selected_row();
void on_faveWindow_destroy();
void on_flashWindow_destroy();
gboolean foreach_func (GtkTreeModel *model,GtkTreePath *path, GtkTreeIter *iter,GList **rowref_list);


enum{
    COLUMN_ENG,
    COLUMN_VIE,
    COLUMN_TOGGLE,
    NUM_COLUMNS
};


int callback(void *NotUsed, int argc, char **argv, char **azColName){
    NotUsed = 0;
    data[cur].eng=(char*)malloc(strlen(argv[0])*sizeof(char));
    strcpy(data[cur].eng, argv[0]);
    data[cur].vie=(char*)malloc(strlen(argv[1])*sizeof(char));
    strcpy(data[cur].vie, argv[1]);
    cur++;
    return 0;
}


GtkTreeIter curIter;
GtkListStore *store;

GtkTreeModel *create_model(Dict dataParam[]){
    gint i=0;
    store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING,G_TYPE_BOOLEAN);
    for(i = 0; i < cur; ++i){
        gtk_list_store_append(store, &curIter);
        gtk_list_store_set(store, &curIter, COLUMN_ENG, dataParam[i].eng, 
                                            COLUMN_VIE, dataParam[i].vie, COLUMN_TOGGLE, FALSE,-1);
    }
    return GTK_TREE_MODEL(store);
}

GtkTreeViewColumn *column1, *column2, *column3;
GtkCellRenderer *renderer,*renderer1;

void add_book_columns(GtkTreeView *treeview){

    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    renderer = gtk_cell_renderer_text_new();
    renderer1 = gtk_cell_renderer_toggle_new();
    column1 = gtk_tree_view_column_new_with_attributes("English", renderer, "text", COLUMN_ENG, NULL);
    column2 = gtk_tree_view_column_new_with_attributes("Vietnamese", renderer, "text", COLUMN_VIE, NULL);
    column3 = gtk_tree_view_column_new_with_attributes("Select", renderer1, "active", COLUMN_TOGGLE, NULL);
    gtk_tree_view_append_column(treeview, column1);
    gtk_tree_view_append_column(treeview, column2);
    gtk_tree_view_append_column(treeview, column3);

    gtk_tree_view_column_set_visible(column3,FALSE);
}

sqlite3 *db;
int main(int argc, char *argv[]){
    char *err_msg = 0;
    int rc = sqlite3_open("dict.db", &db);
    if(rc != SQLITE_OK){
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql = "SELECT * FROM Dict ORDER BY English";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    if (argc < 2) {
        g_print("Usage: ./textview textview.glade\n");
        return 1;
    }
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file(argv[1]);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window-main"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), "NULL");
    gtk_builder_connect_signals(builder, NULL);

    mybookview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "dictionary-view"));
    search = GTK_WIDGET(gtk_builder_get_object(builder, "search"));
    fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
    scroll1 = GTK_WIDGET(gtk_builder_get_object(builder, "scroll1"));
    selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder, "selection"));
    addPop = GTK_WIDGET(gtk_builder_get_object(builder, "addPop"));
    editPop = GTK_WIDGET(gtk_builder_get_object(builder, "editPop"));
    viewPop = GTK_WIDGET(gtk_builder_get_object(builder, "viewPop"));
    deleteBtn = GTK_WIDGET(gtk_builder_get_object(builder, "deleteBtn"));
    confirmBtn = GTK_WIDGET(gtk_builder_get_object(builder, "confirmBtn"));
    cancelBtn = GTK_WIDGET(gtk_builder_get_object(builder, "cancelBtn"));
    box = GTK_WIDGET(gtk_builder_get_object(builder, "box"));
    titleLabel = GTK_WIDGET(gtk_builder_get_object(builder, "titleLabel"));
    historyPop = GTK_WIDGET(gtk_builder_get_object(builder, "historyPop"));
    favePop = GTK_WIDGET(gtk_builder_get_object(builder, "favePop"));
    // notiLabel2 = GTK_WIDGET(gtk_builder_get_object(builder, "notiLabel2"));

    gtk_widget_set_sensitive(editPop, FALSE);
    gtk_widget_set_sensitive(viewPop, FALSE);

    g_signal_connect(mybookview, "row-activated", G_CALLBACK(onRowActivated), NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(mybookview), GTK_TREE_MODEL(create_model(data)));
    add_book_columns(GTK_TREE_VIEW(mybookview));

    history = gtk_list_store_new(1, G_TYPE_STRING);
    fave = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

    GtkEntryCompletion *completion = gtk_entry_completion_new();
    
    gtk_entry_completion_set_model(completion, gtk_tree_view_get_model(mybookview));

    gtk_entry_completion_set_text_column (completion, 0);

    gtk_entry_set_completion(GTK_ENTRY(search), completion);
    gtk_widget_show(window);
    gtk_main();
    sqlite3_close(db);
    return 0;
}

//TIM KIEM

int rowNbr;
int rowMax;
void on_search_search_changed(GtkSearchEntry *s){
    const gchar *srch;
    GtkTreeIter iter;

    GtkTreeModel *model = gtk_tree_view_get_model(mybookview);

    srch = gtk_entry_get_text(GTK_ENTRY(s));

    if(strlen(srch) == 0) return;

    printf("\nSearch for: %s\n", srch);

    gtk_tree_model_get_iter_first(model, &iter);
    
    rowNbr = 0;
    findRow(model, iter, srch);
}

int findRow(GtkTreeModel *model, GtkTreeIter iter, const gchar *srch){
    const gchar *text;
    GtkTreeIter iterChild;
    GtkTreePath *path;

    while(1){
        gtk_tree_model_get(model, &iter, 0, &text, -1); //get col 1
        rowNbr++; //row number

        if(strncasecmp(text, srch, strlen(srch)) == 0){//case insensitive search
            printf("Found: %d %s\n", rowNbr, text);
            path = gtk_tree_model_get_path(model, &iter);
            gtk_tree_view_expand_all(mybookview);
            gtk_tree_view_scroll_to_cell(mybookview, path, NULL, TRUE, 0.5, 0.0);
            gtk_tree_view_set_cursor_on_cell(mybookview, path, NULL, NULL, FALSE);
            return 1;
        }
        if(!gtk_tree_model_iter_next(model, &iter)){
            return 0;
        }
    }
}

//THEM MOI

void on_addPop_clicked(GtkButton *b){
    gtk_widget_set_sensitive(addPop, FALSE);
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    addWindow = GTK_WIDGET(gtk_builder_get_object(builder, "addWindow"));
    g_signal_connect(addWindow, "destroy", G_CALLBACK(on_addWindow_destroy), "NULL");
    gtk_builder_connect_signals(builder, NULL);
    fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
    scroll3 = GTK_WIDGET(gtk_builder_get_object(builder, "scroll3"));
    notiLabel1 = GTK_WIDGET(gtk_builder_get_object(builder, "notiLabel1"));
    engEntry3 = GTK_WIDGET(gtk_builder_get_object(builder, "engEntry3"));
    addBtn1 = GTK_WIDGET(gtk_builder_get_object(builder, "addBtn1"));
    viewport1 = GTK_WIDGET(gtk_builder_get_object(builder, "viewport1"));
    textview1 = GTK_WIDGET(gtk_builder_get_object(builder, "textview1"));
    TextBuffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview1));
    g_signal_connect(TextBuffer1, "changed", G_CALLBACK(on_changed_text1), NULL);
    gtk_widget_show(addWindow);
}

void on_addWindow_destroy(){
    gtk_widget_destroy(addWindow);
    gtk_widget_set_sensitive(addPop, TRUE);
}
void on_engEntry3_changed(GtkEntry *e){
    sprintf(eng, "%s", gtk_entry_get_text(e));
}

void on_changed_text1(GtkTextBuffer *t){
    gtk_widget_show(GTK_WIDGET(addBtn1));
}

void on_addBtn1_clicked(GtkButton *b){
    GtkTextIter begin, end;
    gchar *meaning;

    gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(TextBuffer1), &begin, (gint) 0);
    gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(TextBuffer1), &end, (gint) -1);

    meaning = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(TextBuffer1), &begin, &end, TRUE);

    if(strlen(eng)==0 || strlen(meaning)==0){
        gtk_label_set_text(GTK_LABEL(notiLabel1), (const gchar*)"Du lieu nhap vao khong hop le");
        return;
    }
    if(islower(eng[0])){
        eng[0]-=32;
    }
    sprintf(sqlStm, "INSERT INTO Dict VALUES('%s', \"%s\");", eng, meaning);
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sqlStm, 0, 0, &err_msg);
    if(rc!=SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", err_msg);
        gtk_label_set_text(GTK_LABEL(notiLabel1), (const gchar*)err_msg);
        sqlite3_free(err_msg);
        return;
    }
    int position=0;
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(mybookview);
    gtk_tree_model_get_iter_first(model, &iter);
    const gchar *engWord;
    GtkTreePath *path;
    while(1){
        gtk_tree_model_get(model, &iter, 0, &engWord, -1);
        if(strcasecmp(engWord, eng) > 0){
            gtk_list_store_insert(store, &iter, (gint) position);
            gtk_list_store_set(store, &iter, COLUMN_ENG, eng, COLUMN_VIE, meaning, -1);
            break;
        }
        position++;
        if(!gtk_tree_model_iter_next(model, &iter)){
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, COLUMN_ENG, eng, 
                                            COLUMN_VIE, meaning, COLUMN_TOGGLE, FALSE,-1);
        }
    }
    gtk_label_set_text(GTK_LABEL(notiLabel1), (const gchar*)"New word has been added");
    cur++;
    return;
}

//SUA
char formerEng[1000];
char formerVie[1000];
GtkTreeIter selectIter;

void on_editPop_clicked(GtkButton *b){
    gtk_widget_set_sensitive(editPop, FALSE);
    gtk_widget_set_sensitive(viewPop, FALSE);
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    editWindow = GTK_WIDGET(gtk_builder_get_object(builder, "editWindow"));

    g_signal_connect(editWindow, "destroy", G_CALLBACK(on_editWindow_destroy), "NULL");
    
    gtk_builder_connect_signals(builder, NULL);
    fixed3 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed3"));
    scroll2 = GTK_WIDGET(gtk_builder_get_object(builder, "scroll2"));
    notiLabel3 = GTK_WIDGET(gtk_builder_get_object(builder, "notiLabel3"));
    engEntry1 = GTK_WIDGET(gtk_builder_get_object(builder, "engEntry1"));
    saveBtn = GTK_WIDGET(gtk_builder_get_object(builder, "saveBtn"));
    viewport2 = GTK_WIDGET(gtk_builder_get_object(builder, "viewport2"));
    textview2 = GTK_WIDGET(gtk_builder_get_object(builder, "textview2"));
    TextBuffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2));
    g_signal_connect(TextBuffer2, "changed", G_CALLBACK(on_changed_text2), NULL);
    gtk_entry_set_text(GTK_ENTRY(engEntry1), (const gchar*)formerEng);
    gtk_text_buffer_set_text(TextBuffer2, (const gchar*)formerVie, (gint) -1);
    gtk_widget_show(editWindow);
    gtk_widget_hide(saveBtn);
}

void on_changed_text2(GtkTextBuffer *t){
    gtk_widget_show(saveBtn);
}

void on_editWindow_destroy(){
    gtk_widget_destroy(editWindow);
    gtk_widget_set_sensitive(editPop, TRUE);
    gtk_widget_set_sensitive(viewPop, TRUE);
}

void on_engEntry1_changed(GtkEntry *e){
    gtk_widget_show(saveBtn);
}


void on_selection_changed(GtkWidget *c){
    gchar *value;
    gboolean box;

    GtkTreeModel *selectModel;
    if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(c), &selectModel, &selectIter) == FALSE)
    return;
    gtk_widget_set_sensitive(editPop, TRUE);
    gtk_widget_set_sensitive(viewPop, TRUE);
    gtk_tree_model_get(selectModel, &selectIter, 0, &value, -1);
    printf("English: %s\n", value);
    strcpy(formerEng, (char*)value);
    gtk_tree_model_get(selectModel, &selectIter, 1, &value, -1);
    strcpy(formerVie, (char*)value);
    printf("Vietnamese: %s\n", value);

    gtk_tree_model_get(selectModel, &selectIter, COLUMN_TOGGLE, &box, -1);
    if(box == TRUE)
    {
        box = FALSE;
    }
    else
    {
        box = TRUE;
    }
    if(!gtk_widget_is_visible(deleteBtn))
    {
        gtk_list_store_set(store,&selectIter,COLUMN_TOGGLE,box,-1);
    }
    
}

void on_saveBtn_clicked(GtkButton *b){
    gchar *engUpd, *vieUpd;
    GtkTextIter begin, end;
    engUpd = gtk_entry_get_text(GTK_ENTRY(engEntry1));

    gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(TextBuffer2), &begin, (gint) 0);
    gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(TextBuffer2), &end, (gint) -1);

    vieUpd = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(TextBuffer2), &begin, &end, TRUE);
    if(strlen(engUpd) == 0 || strlen(vieUpd) == 0){
        gtk_label_set_text(GTK_LABEL(notiLabel3), (const gchar*)"Invalid input");
    }
    if(islower(engUpd[0])){
        engUpd[0]-=32;
    }
    sprintf(sqlStm, "UPDATE Dict SET English = '%s', Vietnamese = \"%s\" WHERE English = '%s';", engUpd, vieUpd, formerEng);
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sqlStm, 0, 0, &err_msg);
    if(rc!=SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", err_msg);
        gtk_label_set_text(GTK_LABEL(notiLabel3), (const gchar*)err_msg);
        sqlite3_free(err_msg);
        return;
    }
    gtk_widget_hide(saveBtn);
    gtk_label_set_text(GTK_LABEL(notiLabel3), (const gchar*)"Updated successfully");
    gtk_list_store_set(store, &selectIter, 0, (const gchar*)engUpd, -1);
    gtk_list_store_set(store, &selectIter, 1, (const gchar*)vieUpd, -1);
}

//XEM
void on_viewPop_clicked(GtkButton *b){
    gtk_widget_set_sensitive(editPop, FALSE);
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    editWindow = GTK_WIDGET(gtk_builder_get_object(builder, "editWindow"));
    g_signal_connect(editWindow, "destroy", G_CALLBACK(on_editWindow_destroy), "NULL");
    gtk_builder_connect_signals(builder, NULL);
    fixed3 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed3"));
    scroll2 = GTK_WIDGET(gtk_builder_get_object(builder, "scroll2"));
    notiLabel3 = GTK_WIDGET(gtk_builder_get_object(builder, "notiLabel3"));
    engEntry1 = GTK_WIDGET(gtk_builder_get_object(builder, "engEntry1"));
    saveBtn = GTK_WIDGET(gtk_builder_get_object(builder, "saveBtn"));
    viewport2 = GTK_WIDGET(gtk_builder_get_object(builder, "viewport2"));
    textview2 = GTK_WIDGET(gtk_builder_get_object(builder, "textview2"));
    TextBuffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview2));
    g_signal_connect(TextBuffer2, "changed", G_CALLBACK(on_changed_text2), NULL);
    gtk_entry_set_text(GTK_ENTRY(engEntry1), (const gchar*)formerEng);
    gtk_text_buffer_set_text(TextBuffer2, (const gchar*)formerVie, (gint) -1);
    char tmp[255];
    sprintf(tmp, "%s\n", formerEng);
    gtk_list_store_prepend(history, &hisiter);
    gtk_list_store_set(history, &hisiter, 0, tmp, -1);
    hiscount++;
    gtk_widget_set_sensitive(viewPop, FALSE);
    gtk_widget_show(editWindow);
    gtk_widget_hide(saveBtn);
}

//Xem lich su
void on_historyWindow_destroy();
void on_historyPop_clicked()
{
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    historyWindow = GTK_WIDGET(gtk_builder_get_object(builder, "historyWindow"));

    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(editWindow, "destroy", G_CALLBACK(on_historyWindow_destroy), "NULL");
    fixed4 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed4"));
    historylabel = GTK_WIDGET(gtk_builder_get_object(builder, "historylabel"));
    historyscroll = GTK_WIDGET(gtk_builder_get_object(builder, "historyscroll"));
    historyview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "history-view"));

    gtk_tree_view_set_model(GTK_TREE_VIEW(historyview), GTK_TREE_MODEL(history));
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkTreeModel *model = gtk_tree_view_get_model(historyview);
    renderer = gtk_cell_renderer_text_new();
    col = gtk_tree_view_column_new_with_attributes("Tu da tra", renderer, "text", COLUMN_ENG, NULL);
    gtk_tree_view_append_column(historyview, col);

    //g_signal_connect(mybookview, "row-activated", G_CALLBACK(onRowActivated), NULL);

    GtkEntryCompletion *completion = gtk_entry_completion_new();
    
    gtk_entry_completion_set_model(completion, gtk_tree_view_get_model(historyview));

    gtk_entry_completion_set_text_column (completion, 0);
    gtk_widget_set_sensitive(historyPop, FALSE);
    gtk_widget_show(historyWindow);
}

void on_historyWindow_destroy(){
    gtk_widget_destroy(historyWindow);
    gtk_widget_set_sensitive(historyPop, TRUE);
}

// XOA VERSION 2

void	on_cr2_toggled ( GtkTreeView *treeview,GtkCellRendererToggle *cell, gchar *path_string) { 
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean t = FALSE;
	gchar *text;

	printf("---------------------------------------\n");

	printf("box toggle signal received: path = \"%s\"\n", path_string); // path gives row and child data

	model = gtk_tree_view_get_model(treeview); // get the tree model

	gtk_tree_model_get_iter_from_string (model, &iter, path_string); // get iter from path
    
	gtk_tree_model_get(model, &iter, 0, &text, -1); // get the text pointer of col 0

	printf ("For row text  = \"%s\"\n", text);

	gtk_tree_model_get(model, &iter, 1, &t, -1); // get the boolean value of col 1

	if (t == FALSE) t=TRUE; else t=FALSE; // toggle
    gtk_cell_renderer_toggle_set_active(cell,!t);
	// gtk_tree_store_set(store, &iter, 1, t, -1); // alter col 1 check box

	printf("---------------------------------------\n");

	return;
	}

void on_deleteBtn_clicked()
{
    gtk_widget_set_visible(GTK_WIDGET(confirmBtn),TRUE);
    gtk_widget_set_visible(GTK_WIDGET(cancelBtn),TRUE);
    gtk_widget_set_visible(GTK_WIDGET(deleteBtn),FALSE);
    gtk_tree_view_column_set_visible(column3,TRUE);
    // gtk_widget_set_sensitive(addWindow,FALSE);
}

void on_confirmBtn_clicked()
{
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    messageDialog = GTK_WIDGET(gtk_builder_get_object(builder, "messageDialog"));
    messageBox = GTK_WIDGET(gtk_builder_get_object(builder, "messageBox"));
    massageBtn1 = GTK_WIDGET(gtk_builder_get_object(builder, "massageBtn1"));
    massageBtn2 = GTK_WIDGET(gtk_builder_get_object(builder, "massageBtn2"));
    messageLabel = GTK_WIDGET(gtk_builder_get_object(builder, "messageLabel"));
    gtk_builder_connect_signals(builder, NULL);

    gtk_widget_show(messageDialog);
    gtk_widget_set_sensitive(window,FALSE);
}

void on_cancelBtn_clicked()
{
    gtk_widget_set_visible(GTK_WIDGET(confirmBtn),FALSE);
    gtk_widget_set_visible(GTK_WIDGET(cancelBtn),FALSE);
    gtk_widget_set_visible(GTK_WIDGET(deleteBtn),TRUE);
    gtk_tree_view_column_set_visible(column3,FALSE);
}

void on_massageBtn1_clicked()
{
    builder = gtk_builder_new_from_file("dictionary-view.glade");

    checkWindow = GTK_WIDGET(gtk_builder_get_object(builder, "checkWindow"));
    checkFixed = GTK_WIDGET(gtk_builder_get_object(builder, "checkFixed"));
    checkLabel = GTK_WIDGET(gtk_builder_get_object(builder, "checkLabel"));
    checkImage = GTK_WIDGET(gtk_builder_get_object(builder, "checkImage"));
    checkBtn = GTK_WIDGET(gtk_builder_get_object(builder, "checkBtn"));
    gtk_builder_connect_signals(builder, NULL);

    gtk_widget_show(checkWindow);
    gtk_widget_set_sensitive(window,FALSE);
    gtk_widget_destroy(messageDialog);
}

void on_massageBtn2_clicked()
{
    gtk_widget_destroy(messageDialog);
    gtk_widget_set_sensitive(window,TRUE);
    gtk_widget_set_sensitive(addWindow,TRUE);
}

void on_checkBtn_clicked(/*GtkTreeView *treeview*/)
{
    remove_selected_row();
    gtk_tree_view_column_set_visible(column3,FALSE);
    gtk_widget_set_visible(GTK_WIDGET(confirmBtn),FALSE);
    gtk_widget_set_visible(GTK_WIDGET(deleteBtn),TRUE);
    gtk_widget_set_visible(GTK_WIDGET(cancelBtn),FALSE);

    gtk_widget_destroy(checkWindow);
    gtk_widget_set_sensitive(window,TRUE);
    gtk_widget_set_sensitive(addWindow,TRUE);
}

gboolean foreach_func(GtkTreeModel *model,GtkTreePath *path, GtkTreeIter *iter,GList **rowref_list)
{
    gboolean box;
    g_assert(rowref_list!=NULL);

    gtk_tree_model_get(model, iter, COLUMN_TOGGLE, &box, -1);

    if(box==TRUE)
    {
      GtkTreeRowReference *rowref;
      rowref = gtk_tree_row_reference_new(model, path);
      *rowref_list = g_list_append(*rowref_list, rowref);
    }

    return FALSE;
}

void remove_selected_row(/*GtkTreeView *view*/)
{
    GList *rr_list = NULL;
    GList *node;
    GtkTreeModel *model;

    // model = gtk_tree_view_get_model(view);

    gtk_tree_model_foreach(GTK_TREE_MODEL(store),(GtkTreeModelForeachFunc) foreach_func, &rr_list);

    for(node=rr_list;node != NULL;node = node->next)
    {
        GtkTreePath *path;
        path = gtk_tree_row_reference_get_path((GtkTreeRowReference*)node->data);

        if(path)
        {
           GtkTreeIter iter;

           if(gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path))
           {
            //    const gchar *wordId;
            //     gtk_tree_model_get(model, &iter, 0, &wordId, -1);
            //     printf("%s\n", wordId);
            //     sprintf(sqlStm, "DELETE FROM Dict WHERE English = '%s';", (char*)wordId);
            //     char *err_msg = 0;
            //     int rc = sqlite3_exec(db, sqlStm, 0, 0, &err_msg);
            //     if(rc!=SQLITE_OK){
            //         fprintf(stderr, "SQL error: %s\n", err_msg);
            //         sqlite3_free(err_msg);
            //         return;
            //     }
            gtk_list_store_remove(store, &iter);
           }
        }
     }

    g_list_foreach(rr_list, (GFunc) gtk_tree_row_reference_free, NULL);
    g_list_free(rr_list);
}

//Them vao favorite
static void onRowActivated (GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    g_print ("Row has been double-clicked. Adding to favorite.\n");

    model = gtk_tree_view_get_model(view);

    if (!gtk_tree_model_get_iter(model, &iter, path))
        return;

    GtkTreeIter faveiter;
    

    const gchar *eng, *vie;
    gtk_tree_model_get(model, &iter, 0, &eng, -1);
    gtk_tree_model_get(model, &iter, 1, &vie, -1);

    char tmp[100];
    sprintf(tmp, "%s has been added to Favorites", eng);
    // gtk_label_set_text(GTK_LABEL(notiLabel2), (const char*)tmp);

    gtk_list_store_prepend(fave, &faveiter);
    gtk_list_store_set(fave, &faveiter, 0, eng, 1, vie, -1);
    faveCount++;
}

//Favorite

void on_favePop_clicked()
{
    gtk_widget_set_sensitive(favePop, FALSE);
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    faveWindow = GTK_WIDGET(gtk_builder_get_object(builder, "faveWindow"));

    gtk_builder_connect_signals(builder, NULL);

    favefixed = GTK_WIDGET(gtk_builder_get_object(builder, "favefixed"));
    favesearch = GTK_WIDGET(gtk_builder_get_object(builder, "favesearch"));
    favescroll = GTK_WIDGET(gtk_builder_get_object(builder, "favescroll"));
    faveview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "faveview"));
    flashcardPop = GTK_WIDGET(gtk_builder_get_object(builder, "flashcardPop"));

    gtk_tree_view_set_model(GTK_TREE_VIEW(faveview), GTK_TREE_MODEL(fave));
    GtkTreeViewColumn *col1, *col2;
    GtkCellRenderer *renderer;
    GtkTreeModel *model = gtk_tree_view_get_model(faveview);
    renderer = gtk_cell_renderer_text_new();
    col1 = gtk_tree_view_column_new_with_attributes("English", renderer, "text", COLUMN_ENG, NULL);
    col2 = gtk_tree_view_column_new_with_attributes("Vietnamese", renderer, "text", COLUMN_VIE, NULL);
    gtk_tree_view_append_column(faveview, col1);
    gtk_tree_view_append_column(faveview, col2);


    g_signal_connect(faveWindow, "destroy", G_CALLBACK(on_faveWindow_destroy), "NULL");

    //g_signal_connect(historyview, "row-activated", G_CALLBACK(onRowActivated), NULL);

    GtkEntryCompletion *completion = gtk_entry_completion_new();
    
    gtk_entry_completion_set_model(completion, gtk_tree_view_get_model(faveview));

    gtk_entry_completion_set_text_column (completion, 0);

    gtk_entry_set_completion(GTK_ENTRY(favesearch), completion);

    gtk_widget_set_sensitive(favePop, FALSE);

    gtk_widget_show(faveWindow);
}

void on_faveWindow_destroy(){
    gtk_widget_destroy(faveWindow);
    gtk_widget_set_sensitive(favePop, TRUE);
}

//Flashcard
void on_flashcardPop_clicked()
{
    gtk_widget_set_sensitive(favePop, FALSE);
    builder = gtk_builder_new_from_file("dictionary-view.glade");
    flashWindow = GTK_WIDGET(gtk_builder_get_object(builder, "flashWindow"));   
    gtk_builder_connect_signals(builder, NULL);

    flashBtn = GTK_WIDGET(gtk_builder_get_object(builder, "flashBtn"));
    nextBtn = GTK_WIDGET(gtk_builder_get_object(builder, "nextBtn"));

    GtkTreeIter flashIter;
    if(!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(fave), &flashIter)){
        // gtk_widget_set_sensitive(flashcardPop, FALSE);
        return;
    }
    
    const gchar *eng;
    int num = rand() % faveCount + 1;
    for(int i=0; i<num; i++){
        if(!gtk_tree_model_iter_next(GTK_TREE_MODEL(fave), &flashIter)){
            return ;
        }
    }
    gtk_tree_model_get(GTK_TREE_MODEL(fave), &flashIter, 0, &eng, -1);
    ansIter = flashIter;
    anscol = 0;
    gtk_button_set_label(GTK_BUTTON(flashBtn), eng);

    gtk_widget_set_sensitive(flashcardPop, FALSE);

    g_signal_connect(flashWindow, "destroy", G_CALLBACK(on_flashWindow_destroy), "NULL");

    gtk_widget_show(flashWindow);
}

void on_favesearch_search_changed(GtkSearchEntry *s){
    const gchar *text;
    GtkTreeIter faveiter;
    GtkTreePath *path;


    const gchar *srch = gtk_entry_get_text(GTK_ENTRY(s));

    if(strlen(srch) == 0) return;

    g_print("Looking for %s\n", srch);
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(fave), &faveiter);

    while(1){
        gtk_tree_model_get(GTK_TREE_MODEL(fave), &faveiter, 0, &text, -1);
        if(strncasecmp(text, srch, strlen(text)) == 0){
            path = gtk_tree_model_get_path(GTK_TREE_MODEL(fave), &faveiter);
            gtk_tree_view_expand_all(faveview);
            gtk_tree_view_scroll_to_cell(faveview, path, NULL, TRUE, 0.5, 0.0);
            gtk_tree_view_set_cursor_on_cell(faveview, path, NULL, NULL, FALSE);
            return;
        }
        if(!gtk_tree_model_iter_next(GTK_TREE_MODEL(fave), &faveiter)){
            return;
        }
    }
}

void on_flashWindow_destroy()
{
    gtk_widget_destroy(flashWindow);
    gtk_widget_set_sensitive(favePop, TRUE);
    gtk_widget_set_sensitive(flashcardPop, TRUE);
}

void on_flashBtn_clicked()
{
    const gchar *text;

    if(anscol == 0)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(fave), &ansIter, 1, &text, -1); 
        gtk_button_set_label(GTK_BUTTON(flashBtn), text);
        anscol = 1;
        return;
    }
    if(anscol == 1)
    {
        gtk_tree_model_get(GTK_TREE_MODEL(fave), &ansIter, 0, &text, -1); 
        gtk_button_set_label(GTK_BUTTON(flashBtn), text);
        anscol = 0;
        return;
    }
}

int num = 0;
void on_nextBtn_clicked()
{   
    GtkTreeIter flashIter;
    const gchar *eng;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(fave), &flashIter);
    // int num = rand() % faveCount + 1;
    if(num==faveCount)
    num=0;
    for(int i=0; i<num; i++){
        if(!gtk_tree_model_iter_next(GTK_TREE_MODEL(fave), &flashIter)){
            return ;
        }
    }
    gtk_tree_model_get(GTK_TREE_MODEL(fave), &flashIter, 0, &eng, -1);
    ansIter = flashIter;
    anscol = 0;
    gtk_button_set_label(GTK_BUTTON(flashBtn), eng);
    num++;
}
