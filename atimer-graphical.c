/*
  Author: Gustavo Henrique Aguilar
  Date  : 14/02/18
*/

#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<time.h>

#include<gtk/gtk.h>
#include<gdk/gdk.h>

GtkBuilder *builder;
GtkWidget *window;

GtkLabel *lTime;

GtkButton *bStartPause;
GtkButton *bDelete;

GtkButton *bHourUp;
GtkButton *bHourDown;
GtkButton *bMinuteUp;
GtkButton *bMinuteDown;
GtkButton *bSecondUp;
GtkButton *bSecondDown;

GtkComboBoxText *comboBox;

GtkEntry *entryAction;

int strComp(char str1[], char str2[]);
int strToInt(char str[]);
void intToStr(int n, char* auxStr);
void timer(int hours, int minutes, int seconds);
void buttonAction(GtkButton *button, gpointer data);
void on_change(GtkComboBox *cb, gpointer data);
void updateTime(GtkLabel *lTime, int hour, int minute, int second);

//global variables
int hours = 0, minutes = 0, seconds = 0, state = 0; //0-Stoped 1-Paused 2-Running 4-STOPED
char command[256] = "notify-send aTimer FINISHED";
gboolean customAction = FALSE;

int main(int argc, char *argv[]) {
  char strTime[8]; //time to be displayed
  const char *actions[] = {"Power Off", "Reboot", "Custom"};

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "gui.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);//window.connect("destroy", gtk.main_quit) reference with OO

  lTime = GTK_LABEL(gtk_builder_get_object(builder, "lTime"));
  strcpy(strTime, "00:00:00");
  gtk_label_set_text(lTime, strTime);

  bHourUp = GTK_BUTTON(gtk_builder_get_object(builder, "bHourUp"));
  g_signal_connect(bHourUp, "clicked", G_CALLBACK(buttonAction), "bHourUp");
  bHourDown = GTK_BUTTON(gtk_builder_get_object(builder, "bHourDown"));
  g_signal_connect(bHourDown, "clicked", G_CALLBACK(buttonAction), "bHourDown");
  bMinuteUp = GTK_BUTTON(gtk_builder_get_object(builder, "bMinuteUp"));
  g_signal_connect(bMinuteUp, "clicked", G_CALLBACK(buttonAction), "bMinuteUp");
  bMinuteDown = GTK_BUTTON(gtk_builder_get_object(builder, "bMinuteDown"));
  g_signal_connect(bMinuteDown, "clicked", G_CALLBACK(buttonAction), "bMinuteDown");
  bSecondUp = GTK_BUTTON(gtk_builder_get_object(builder, "bSecondUp"));
  g_signal_connect(bSecondUp, "clicked", G_CALLBACK(buttonAction), "bSecondUp");
  bSecondDown = GTK_BUTTON(gtk_builder_get_object(builder, "bSecondDown"));
  g_signal_connect(bSecondDown, "clicked", G_CALLBACK(buttonAction), "bSecondDown");

  bStartPause = GTK_BUTTON(gtk_builder_get_object(builder, "bStartPause"));
  g_signal_connect(bStartPause, "clicked", G_CALLBACK(buttonAction), "bStartPause");
  bDelete = GTK_BUTTON(gtk_builder_get_object(builder, "bDelete"));
  g_signal_connect(bDelete, "clicked", G_CALLBACK(buttonAction), "bDelete");

  comboBox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "cbActions"));
  g_signal_connect(comboBox, "changed", G_CALLBACK(on_change), NULL);
  int i;
  for (i=0; i<G_N_ELEMENTS (actions); i++){
   gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (comboBox), actions[i]);
  }

  entryAction = GTK_ENTRY(gtk_builder_get_object(builder, "entryAction"));
  gtk_widget_hide((GtkWidget*)entryAction);

  gtk_builder_connect_signals(builder, NULL);

  g_object_unref(builder);
  gtk_widget_show(window);
  gtk_main();
  return 0;
}

void on_window_main_destroy(){
  gtk_main_quit();
}
//Compare two strings
int strComp(char str1[], char str2[]){
  int i=0, j=0;
  while(str1[i] != '\x0' && str2[j] != '\x0'){

    if(str1[i] != str2[j]) return 1;
    i++;
    j++;
  }
  if(str1[i] == '\x0' && str2[j] == '\x0'){
    return 0;
  }else{
    return 1;
  }
}
//Convert string to integer
int strToInt(char str[]){
  int n=0, size = strlen(str), i;
  for(i=0; i<size; i++){
    n = n*10+(str[i]-'0');
  }
  return n;
}
// Timer counter, execute the action at the end
void timer(int hours, int minutes, int seconds){
  g_print("Started Timer\n");
  time_t start, end;
  double elapsed;

  while(state != 0){
    time(&start);
    while(state == 1){
        elapsed = 0;
        usleep(1);
        while (gtk_events_pending())
        gtk_main_iteration();
    }
    do{
      time(&end);
      usleep(1);
      elapsed = difftime(end, start);
        while (gtk_events_pending())
        gtk_main_iteration();
    }while(elapsed < 1);
    g_print("%d:%d:%d\n", hours, minutes, seconds);
    if(seconds == 0){
      if(minutes > 0){
        minutes--;
        seconds=60;
      }else{
        if(hours > 0){
          hours--;
          minutes = 60;
        }else{
          state = 0;
          gtk_button_set_label(bStartPause, "Start");
          break;
        }
      }
    }else{
      seconds--;
    }
    if(state == 4) return;
    updateTime(lTime, hours, minutes, seconds);
    elapsed = 0;
  }
  g_print("Time Over!\n");
  system(command);

}

void buttonAction(GtkButton *button, gpointer data){
  char* strData = (char*) data;
  if(state == 4) state = 0;
  if(!state){
    if(!strComp(strData,"bHourUp")){
      if(hours < 99) hours++;
    }
    if(!strComp(strData,"bHourDown")){
      if(hours > 0) hours--;
    }
    if(!strComp(strData,"bMinuteUp")){
      if(minutes < 60) minutes++;
    }
    if(!strComp(strData,"bMinuteDown")){
      if(minutes > 0) minutes--;
    }
    if(!strComp(strData,"bSecondUp")){
      if(seconds < 60) seconds++;
    }
    if(!strComp(strData,"bSecondDown")){
      if(seconds > 0) seconds--;
    }
	updateTime(lTime, hours, minutes, seconds);
  }
    if(!strComp(strData, "bStartPause")){
      if(state == 0){ //STOPED
        gtk_button_set_label(bStartPause, "Pause");
        state = 2;
	  if(customAction == TRUE){
	  	strcpy(command, (char*) gtk_entry_get_text(entryAction));
	  }

        timer(hours, minutes, seconds);
        updateTime(lTime, hours, minutes, seconds);
      }
      else if(state == 1){ // PAUSED
        state = 2;
        gtk_button_set_label(bStartPause, "Pause");
      }
      else if(state == 2){ // RUNNING
        gtk_button_set_label(bStartPause, "Start");
        state = 1;
      }
    }
    if(!strComp(strData, "bDelete")){
      state =4;
      hours=0;
      minutes=0;
      seconds=0;
      gtk_button_set_label(bStartPause, "Start");
    }
}
//Update the time displayed
void updateTime(GtkLabel *lTime, int hour, int minute, int second){
  char str[8] = "";
  char auxStr[2];

  sprintf(auxStr, "%i", hour);
  if(hour<10)
    strcat(str, "0");
  strcat(str, auxStr);
  strcat(str, ":");
  sprintf(auxStr, "%i", minute);
  if(minute<10)
    strcat(str, "0");
  strcat(str, auxStr);
  strcat(str, ":");
  sprintf(auxStr, "%i", second);
  if(second<10)
    strcat(str, "0");
  strcat(str, auxStr);
  str[8] = '\x0';
  gtk_label_set_text(lTime, str);
}
//actions performed when combo box is changed
void on_change(GtkComboBox *cb, gpointer user_data){
    if(!strComp((char*) gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb)), "Power Off")){
      strcpy(command, "poweroff");
      gtk_widget_hide((GtkWidget*)entryAction);
    }
    if(!strComp((char*) gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb)), "Reboot")){
      strcpy(command, "reboot");
      gtk_widget_hide((GtkWidget*)entryAction);
    }
    if(!strComp((char*) gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb)), "Custom")){
      gtk_widget_show((GtkWidget*)entryAction);
      customAction = TRUE;
    }
}
