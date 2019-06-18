/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Android support utilities
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2015 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/tokenzr.h>

#include <QtAndroidExtras/QAndroidJniObject>

#include "dychart.h"
#include "androidUTIL.h"
#include "OCPN_DataStreamEvent.h"
#include "chart1.h"
#include "AISTargetQueryDialog.h"
#include "AISTargetAlertDialog.h"
#include "AISTargetListDialog.h"
//#include "routeprop.h"
#include "TrackPropDlg.h"
#include "S57QueryDialog.h"
#include "options.h"
#include "routemanagerdialog.h"
#include "chartdb.h"
#include "s52plib.h"
#include "s52utils.h"
#include "s52s57.h"
#include "navutil.h"
#include "TCWin.h"
#include "ocpn_plugin.h"
#include "about.h"
#include "OCPNPlatform.h"
#include "multiplexer.h"
#include "chartdbs.h"
#include "glChartCanvas.h"
#include "chcanv.h"
#include "MarkInfo.h"
#include "RoutePropDlgImpl.h"
#include "MUIBar.h"

const wxString AndroidSuppLicense =
wxT("<br><br>The software included in this product contains copyrighted software that is licensed under the GPL.")
wxT("A copy of that license is shown above.")
wxT("You may obtain the complete Corresponding Source code from us for ")
wxT("a period of three years after our last shipment of this product, ")
wxT("by sending a money order or check for $5 to:<br><br>")
wxT("GPL Compliance Division<br>")
wxT("Dyad Inc.<br>")
wxT("31 Ocean Reef Dr<br>")
wxT("# C101-449<br>")
wxT("Key Largo, FL 33037-5282<br>")
wxT("United States<br><br>")
wxT("Please write “source for OpenCPN Version {insert version here} in the memo line of your payment.<br><br>");


#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double*)&lNaN)
#endif



class androidUtilHandler;



extern MyFrame                  *gFrame;
extern const wxEventType wxEVT_OCPN_DATASTREAM;
//extern const wxEventType wxEVT_DOWNLOAD_EVENT;

wxEvtHandler                    *s_pAndroidNMEAMessageConsumer;
wxEvtHandler                    *s_pAndroidBTNMEAMessageConsumer;

extern AISTargetAlertDialog      *g_pais_alert_dialog_active;
extern AISTargetQueryDialog      *g_pais_query_dialog_active;
extern AISTargetListDialog       *g_pAISTargetList;
//extern MarkInfoImpl              *pMarkPropDialog;
extern RoutePropDlgImpl          *pRoutePropDialog;
extern TrackPropDlg              *pTrackPropDialog;
extern S57QueryDialog            *g_pObjectQueryDialog;
extern options                   *g_options;
extern bool                       g_bSleep;
androidUtilHandler               *g_androidUtilHandler;
extern wxDateTime                 g_start_time;
extern RouteManagerDialog        *pRouteManagerDialog;
extern about                     *g_pAboutDlg;
extern bool                      g_bFullscreen;
extern OCPNPlatform              *g_Platform;

// Static globals
extern ChartDB                   *ChartData;
extern MyConfig                  *pConfig;


//   Preferences globals
extern bool             g_bShowOutlines;
extern bool             g_bShowChartBar;
extern bool             g_bShowDepthUnits;
extern bool             g_bskew_comp;
extern bool             g_bopengl;
extern bool             g_bsmoothpanzoom;
extern bool             g_bShowMag;
extern double           g_UserVar;
extern int              g_chart_zoom_modifier;
extern int              g_NMEAAPBPrecision;
extern wxString         g_TalkerIdText;

extern wxString         *pInit_Chart_Dir;
extern wxArrayOfConnPrm *g_pConnectionParams;
extern bool             g_bfilter_cogsog;
extern int              g_COGFilterSec;
extern int              g_SOGFilterSec;


extern bool             g_bDisplayGrid;

//    AIS Global configuration
extern bool             g_bCPAMax;
extern double           g_CPAMax_NM;
extern bool             g_bCPAWarn;
extern double           g_CPAWarn_NM;
extern bool             g_bTCPA_Max;
extern double           g_TCPA_Max;
extern bool             g_bMarkLost;
extern double           g_MarkLost_Mins;
extern bool             g_bRemoveLost;
extern double           g_RemoveLost_Mins;
extern bool             g_bShowCOG;
extern double           g_ShowCOG_Mins;
extern bool             g_bAISShowTracks;
extern double           g_AISShowTracks_Mins;
extern bool             g_bHideMoored;
extern double           g_ShowMoored_Kts;
extern bool             g_bAIS_CPA_Alert;
extern bool             g_bAIS_CPA_Alert_Audio;
extern wxString         g_sAIS_Alert_Sound_File;
extern bool             g_bAIS_CPA_Alert_Suppress_Moored;
extern bool             g_bShowAreaNotices;
extern bool             g_bDrawAISSize;
extern bool             g_bShowAISName;
extern int              g_Show_Target_Name_Scale;
extern bool             g_bWplIsAprsPosition;

extern int              g_iNavAidRadarRingsNumberVisible;
extern float            g_fNavAidRadarRingsStep;
extern int              g_pNavAidRadarRingsStepUnits;
extern int              g_iWaypointRangeRingsNumber;
extern float            g_fWaypointRangeRingsStep;
extern int              g_iWaypointRangeRingsStepUnits;
extern wxColour         g_colourWaypointRangeRingsColour;
extern bool             g_bWayPointPreventDragging;

extern bool             g_bPreserveScaleOnX;
extern bool             g_bPlayShipsBells;
extern int              g_iSoundDeviceIndex;
extern bool             g_bFullscreenToolbar;

extern int              g_OwnShipIconType;
extern double           g_n_ownship_length_meters;
extern double           g_n_ownship_beam_meters;
extern double           g_n_gps_antenna_offset_y;
extern double           g_n_gps_antenna_offset_x;
extern int              g_n_ownship_min_mm;
extern double           g_n_arrival_circle_radius;

extern bool             g_bEnableZoomToCursor;
extern bool             g_bTrackDaily;
extern bool             g_bHighliteTracks;
extern double           g_TrackIntervalSeconds;
extern double           g_TrackDeltaDistance;
extern double           g_TrackDeltaDistance;
extern int              g_nTrackPrecision;

extern int              g_iSDMMFormat;
extern int              g_iDistanceFormat;
extern int              g_iSpeedFormat;

extern bool             g_bAdvanceRouteWaypointOnArrivalOnly;

extern int              g_cm93_zoom_factor;

extern int              g_COGAvgSec;

extern bool             g_bCourseUp;
extern bool             g_bLookAhead;

extern double           g_ownship_predictor_minutes;
extern double           g_ownship_HDTpredictor_miles;
extern double           gLat, gLon, gCog, gSog, gHdt, gVar;

extern bool             g_bAISRolloverShowClass;
extern bool             g_bAISRolloverShowCOG;
extern bool             g_bAISRolloverShowCPA;

extern bool             g_bAIS_ACK_Timeout;
extern double           g_AckTimeout_Mins;

extern bool             g_bQuiltEnable;
extern bool             g_bFullScreenQuilt;
extern bool             g_bConfirmObjectDelete;
extern wxString         g_GPS_Ident;
extern bool             g_bGarminHostUpload;

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3,0,0)
extern wxLocale         *plocale_def_lang;
#endif

//extern OCPN_Sound        g_anchorwatch_sound;
extern bool             g_bMagneticAPB;

extern bool             g_fog_overzoom;
extern double           g_overzoom_emphasis_base;
extern bool             g_oz_vector_scale;
extern bool             g_bShowStatusBar;

extern ocpnGLOptions    g_GLOptions;


//#ifdef USE_S57
extern s52plib          *ps52plib;
//#endif

extern wxString         g_locale;
extern bool             g_bportable;
extern bool             g_bdisable_opengl;

extern ChartGroupArray  *g_pGroupArray;


extern bool             g_bUIexpert;
//    Some constants
#define ID_CHOICE_NMEA  wxID_HIGHEST + 1

//extern wxArrayString *EnumerateSerialPorts(void);           // in chart1.cpp

extern wxArrayString    TideCurrentDataSet;
extern wxString         g_TCData_Dir;

extern AIS_Decoder      *g_pAIS;
extern bool             g_bserial_access_checked;

extern options          *g_pOptions;

extern bool             g_btouch;
extern bool             g_bresponsive;
extern bool             g_bAutoHideToolbar;
extern int              g_nAutoHideToolbar;
extern int              g_GUIScaleFactor;
extern int              g_ChartScaleFactor;

extern double           g_config_display_size_mm;
extern float            g_ChartScaleFactorExp;

extern Multiplexer      *g_pMUX;
extern bool             b_inCloseWindow;
extern bool             g_config_display_size_manual;
extern MarkInfoDlg      *g_pMarkInfoDialog;

WX_DEFINE_ARRAY_PTR(ChartCanvas*, arrayofCanvasPtr);
extern arrayofCanvasPtr g_canvasArray;

wxString callActivityMethod_vs(const char *method);


//      Globals, accessible only to this module

JavaVM *java_vm;
JNIEnv* global_jenv;
bool     b_androidBusyShown;
double   g_androidDPmm;
double   g_androidDensity;

bool            g_bExternalApp;

wxString        g_androidFilesDir;
wxString        g_androidCacheDir;
wxString        g_androidExtFilesDir;
wxString        g_androidExtCacheDir;
wxString        g_androidExtStorageDir;

int             g_mask;
int             g_sel;
int             g_ActionBarHeight;
bool            g_follow_active;
bool            g_track_active;
bool            bGPSEnabled;

wxSize          config_size;

bool            s_bdownloading;
wxString        s_requested_url;
wxEvtHandler    *s_download_evHandler;
bool            g_running;
bool            g_bstress1;
extern int      g_GUIScaleFactor;

wxString        g_deviceInfo;

int             s_androidMemTotal;
int             s_androidMemUsed;
bool            g_backEnabled;

bool            s_optionsActive;

extern int ShowNavWarning();
extern bool     g_btrackContinuous;

#define ANDROID_EVENT_TIMER 4389
#define ANDROID_STRESS_TIMER 4388
#define ANDROID_RESIZE_TIMER 4387

#define ACTION_NONE                     -1
#define ACTION_RESIZE_PERSISTENTS       1
#define ACTION_FILECHOOSER_END          3
#define ACTION_COLORDIALOG_END          4
#define ACTION_POSTASYNC_END            5

class androidUtilHandler : public wxEvtHandler
{
 public:
     androidUtilHandler();
    ~androidUtilHandler() {}
    
    void onTimerEvent(wxTimerEvent &event);
    void onStressTimer(wxTimerEvent &event);
    void OnResizeTimer(wxTimerEvent &event);
    void OnCmdEvent( wxCommandEvent& event );

    wxString GetStringResult(){ return m_stringResult; }
    
    wxTimer     m_eventTimer;
    int         m_action;
    bool        m_done;
    wxString    m_stringResult;
    wxTimer     m_stressTimer;
    wxTimer     m_resizeTimer;
    int         timer_sequence;
    
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE ( androidUtilHandler, wxEvtHandler )
EVT_TIMER ( ANDROID_EVENT_TIMER, androidUtilHandler::onTimerEvent )
EVT_TIMER ( ANDROID_RESIZE_TIMER, androidUtilHandler::OnResizeTimer )
EVT_MENU(-1, androidUtilHandler::OnCmdEvent)

END_EVENT_TABLE()

androidUtilHandler::androidUtilHandler()
{
    m_eventTimer.SetOwner( this, ANDROID_EVENT_TIMER );
    m_stressTimer.SetOwner( this, ANDROID_STRESS_TIMER );
    m_resizeTimer.SetOwner(this, ANDROID_RESIZE_TIMER);
 
}

       
void androidUtilHandler::OnCmdEvent( wxCommandEvent& event )
{
    switch( event.GetId() ){
        case ID_CMD_TRIGGER_RESIZE:
            timer_sequence = 0;
            m_resizeTimer.Start(10, wxTIMER_ONE_SHOT);
            break;
    }
}



void androidUtilHandler::onTimerEvent(wxTimerEvent &event)
{
//    qDebug() << "onTimerEvent";

    switch(m_action){
        case ACTION_RESIZE_PERSISTENTS:            //  Handle rotation/resizing of persistent dialogs

            // AIS Target Query
            if( g_pais_query_dialog_active ) {
                qDebug() << "AISB";

                bool bshown = g_pais_query_dialog_active->IsShown();
                g_pais_query_dialog_active->Hide();
                g_pais_query_dialog_active->RecalculateSize();
                if(bshown){
                    qDebug() << "AISC";
                    g_pais_query_dialog_active->Show();
                    g_pais_query_dialog_active->Raise();
                }
            }

            // Route Props
            if(RoutePropDlgImpl::getInstanceFlag()){
                qDebug() << "RPDIA";
                bool bshown = pRoutePropDialog->IsShown();
                if(bshown){
                    qDebug() << "RPDIB";
                    pRoutePropDialog->Hide();
                    pRoutePropDialog->RecalculateSize();
                    pRoutePropDialog->Show();
                }
                else{
                    pRoutePropDialog->Destroy();
                    pRoutePropDialog = NULL;
                }
            }
#if 0            
            // Track Props
            if(TrackPropDlg::getInstanceFlag()){
                bool bshown = pTrackPropDialog->IsShown();
                if(bshown){
                    pTrackPropDialog->Hide();
                    pTrackPropDialog->RecalculateSize();
                    pTrackPropDialog->Show();
                }
                else{
                    pTrackPropDialog->Destroy();
                    pTrackPropDialog = NULL;
                }
            }
            
#endif            
            // Mark Props
//             if(MarkInfoImpl::getInstanceFlag()){
//                 bool bshown = pMarkPropDialog->IsShown();
//                 if(bshown){
//                     pMarkPropDialog->Hide();
//                     pMarkPropDialog->RecalculateSize();
//                     pMarkPropDialog->Show();
//                 }
//                 else{
//                     pMarkPropDialog->Destroy();
//                     pMarkPropDialog = NULL;
//                 }
//             }
            
            if(g_pMarkInfoDialog){
                bool bshown = g_pMarkInfoDialog->IsShown();
                g_pMarkInfoDialog->Hide();
                g_pMarkInfoDialog->RecalculateSize();
                if(bshown){
                    g_pMarkInfoDialog->Show();
                }
                
            }
            
            // ENC Object Query
            if(g_pObjectQueryDialog){
                bool bshown = g_pObjectQueryDialog->IsShown();
                g_pObjectQueryDialog->Hide();
                g_pObjectQueryDialog->RecalculateSize();
                if(bshown){
                    g_pObjectQueryDialog->Show();
                } 
            }
            
///v5            // Options dialog
//             if(g_options){
//                 ///v5qDebug() << "optionsA";
//                 bool bshown = g_options->IsActive();
//                 g_options->Hide();
//                 g_options->RecalculateSize();
//                 //if(s_optionsActive)
//                 {
//                     qDebug() << "optionsB";
//                     g_options->Raise();
//                 }
//             }
            
            // AIS Target List dialog
            if(g_pAISTargetList){
                qDebug() << "ATLA";
                bool bshown = g_pAISTargetList->IsShown();
                g_pAISTargetList->Hide();
                g_pAISTargetList->RecalculateSize();
                if(bshown){
                    qDebug() << "ATLB";
                    g_pAISTargetList->Show();
                    g_pAISTargetList->Raise();
                }
            }
 
#if 0
///v5
            // Tide/Current window
            if( cc1 && cc1->getTCWin()){
                bool bshown = cc1->getTCWin()->IsShown();
                cc1->getTCWin()->Hide();
                cc1->getTCWin()->RecalculateSize();
                if(bshown){
                    cc1->getTCWin()->Show();
                    cc1->getTCWin()->Refresh();
                }
            }
#endif            
            // Route Manager dialog
            if(RouteManagerDialog::getInstanceFlag()){
                bool bshown = pRouteManagerDialog->IsShown();
                if(bshown){
                    pRouteManagerDialog->Hide();
                    pRouteManagerDialog->RecalculateSize();
                    pRouteManagerDialog->Show();
                }
                else{
                    pRouteManagerDialog->Destroy();
                    pRouteManagerDialog = NULL;
                }
                    
            }
            
            // About dialog
            if(g_pAboutDlg){
                bool bshown = g_pAboutDlg->IsShown();
                g_pAboutDlg->Hide();
                ///v5g_pAboutDlg->RecalculateSize();
                if(bshown){
                    g_pAboutDlg->Show();
                }
            }
            
            
            break;
 
        case ACTION_FILECHOOSER_END:            //  Handle polling of android Dialog
            {
                //qDebug() << "chooser poll";
                //  Get a reference to the running FileChooser
                QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                "activity", "()Landroid/app/Activity;");
                
                if ( !activity.isValid() ){
                    //qDebug() << "onTimerEvent : Activity is not valid";
                    return;
                }
 
                //  Call the method which tracks the completion of the Intent.
                QAndroidJniObject data = activity.callObjectMethod("isFileChooserFinished", "()Ljava/lang/String;");
                
                jstring s = data.object<jstring>();
                
                JNIEnv* jenv;
                
                //  Need a Java environment to decode the resulting string
                if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
                    //qDebug() << "GetEnv failed.";
                }
                else {
                    
                    // The string coming back will be one of:
                    //  "no"   ......Intent not done yet.
                    //  "cancel:"   .. user cancelled intent.
                    //  "file:{file_name}"  .. user selected this file, fully qualified.
                    if(!s){
                        //qDebug() << "isFileChooserFinished returned null";
                    }
                    else if( (jenv)->GetStringLength( s )){
                        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
//                        qDebug() << "isFileChooserFinished returned " << ret_string;
                        if( !strncmp(ret_string, "cancel:", 7) ){
                            m_done = true;
                            m_stringResult = _T("cancel:");
                        }
                        else if( !strncmp(ret_string, "file:", 5) ){
                            m_done = true;
                            m_stringResult = wxString(ret_string, wxConvUTF8);
                        } 
                    }
                }
                
                
                break;
            }

        case ACTION_COLORDIALOG_END:            //  Handle polling of android Dialog
            {
                //qDebug() << "colorpicker poll";
                //  Get a reference to the running FileChooser
                QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                                       "activity", "()Landroid/app/Activity;");
                
                if ( !activity.isValid() ){
                    //qDebug() << "onTimerEvent : Activity is not valid";
                    return;
                }
                
                //  Call the method which tracks the completion of the Intent.
                QAndroidJniObject data = activity.callObjectMethod("isColorPickerDialogFinished", "()Ljava/lang/String;");
                
                jstring s = data.object<jstring>();
                
                JNIEnv* jenv;
                
                //  Need a Java environment to decode the resulting string
                if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
                    //qDebug() << "GetEnv failed.";
                }
                else {
                    
                    // The string coming back will be one of:
                    //  "no"   ......Dialog not done yet.
                    //  "cancel:"   .. user cancelled Dialog.
                    //  "color: ".
                    if(!s){
///v5                        qDebug() << "isColorPickerDialogFinished returned null";
                    }
                    else if( (jenv)->GetStringLength( s )){
                        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
                        //qDebug() << "isColorPickerDialogFinished returned " << ret_string;
                        if( !strncmp(ret_string, "cancel:", 7) ){
                            m_done = true;
                            m_stringResult = _T("cancel:");
                        }
                        else if( !strncmp(ret_string, "color:", 6) ){
                            m_done = true;
                            m_stringResult = wxString(ret_string, wxConvUTF8);
                        } 
                    }
                }
                
                
                break;
            }
            
        case ACTION_POSTASYNC_END:            //  Handle polling of android async POST task end
            {
                //qDebug() << "colorpicker poll";
                //  Get a reference to the running FileChooser
                QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                                       "activity", "()Landroid/app/Activity;");
                
                if ( !activity.isValid() ){
                    //qDebug() << "onTimerEvent : Activity is not valid";
                    return;
                }
                
                //  Call the method which tracks the completion of the POST async task.
                QAndroidJniObject data = activity.callObjectMethod("checkPostAsync", "()Ljava/lang/String;");
                
                jstring s = data.object<jstring>();
                
                JNIEnv* jenv;
                
                //  Need a Java environment to decode the resulting string
                if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
                    //qDebug() << "GetEnv failed.";
                }
                else {
                    
                    // The string coming back will be either:
                    //  "ACTIVE"   ......Post command not done yet.
                    //  A valid XML response body.
                    if(!s){
///v5                        qDebug() << "checkPostAsync returned null";
                    }
                    else if( (jenv)->GetStringLength( s )){
                        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
                        qDebug() << "checkPostAsync returned " << ret_string;
                        if( strncmp(ret_string, "ACTIVE", 6) ){         // Must be done....
                            m_done = true;
                            m_stringResult =  wxString(ret_string, wxConvUTF8);
                        }
                    }
                }
                
                
                break;
            }
            
            
        default:
            break;
    }
    
}

void androidUtilHandler::OnResizeTimer(wxTimerEvent &event)
{
    if(timer_sequence == 0){
    //  On QT, we need to clear the status bar item texts to prevent the status bar from
    //  growing the parent frame due to unexpected width changes.
//         if( m_pStatusBar != NULL ){
//             int widths[] = { 2,2,2,2,2 };
//            m_pStatusBar->SetStatusWidths( m_StatusBarFieldCount, widths );
// 
//             for(int i=0 ; i <  m_pStatusBar->GetFieldsCount() ; i++){
//                 m_pStatusBar->SetStatusText(_T(""), i);
//             }
//         }
        qDebug() << "sequence 0";

        timer_sequence++;
        //  This timer step needs to be long enough to allow Java induced size change to take effect
        //  in another thread.
        m_resizeTimer.Start(1000, wxTIMER_ONE_SHOT);
        return;
    }



    if(timer_sequence == 1){
        qDebug() << "sequence 1" << config_size.x;
        gFrame->SetSize(config_size);
        timer_sequence++;
        m_resizeTimer.Start(10, wxTIMER_ONE_SHOT);
        return;
    }

    if(timer_sequence == 2){
        timer_sequence++;
        m_resizeTimer.Start(10, wxTIMER_ONE_SHOT);
        return;
    }

    if(timer_sequence == 3){
        qDebug() << "sequence 3";
        androidConfirmSizeCorrection();

        timer_sequence++;
        m_resizeTimer.Start(10, wxTIMER_ONE_SHOT);
        return;
    }
    
    if(timer_sequence == 4){
        qDebug() << "sequence 4";
        
        //  Raise the resized options dialog.
        //  This has no effect if the dialog is not already shown.
        g_options->Raise();

        resizeAndroidPersistents();
        return;
    }

}









int stime;

void androidUtilHandler::onStressTimer(wxTimerEvent &event){

    g_GUIScaleFactor = -5;
    g_ChartScaleFactor = -5;
    ///v5gFrame->SetToolbarScale();
    gFrame->SetGPSCompassScale();
    
    s_androidMemUsed  = 80;
    
    g_GLOptions.m_bTextureCompression = 0;
    g_GLOptions.m_bTextureCompressionCaching = 0;
    
    if(600 == stime++) androidTerminate();
    
}



bool androidUtilInit( void )
{
    qDebug() << "androidUtilInit()";
    
    g_androidUtilHandler = new androidUtilHandler();

    //  Initialize some globals
    
    s_androidMemTotal  = 100;
    s_androidMemUsed  = 50;
    
    wxString dirs = callActivityMethod_vs("getSystemDirs");
    qDebug() << "dirs: " << dirs.mb_str();
    
    wxStringTokenizer tk(dirs, _T(";"));
    if( tk.HasMoreTokens() ){
        wxString token = tk.GetNextToken();
        if(wxNOT_FOUND != token.Find(_T("EXTAPP")))
            g_bExternalApp = true;
        
        token = tk.GetNextToken();              
        g_androidFilesDir = token;              // used for "home dir"
        token = tk.GetNextToken();              
        g_androidCacheDir = token;
        token = tk.GetNextToken();              
        g_androidExtFilesDir = token;           // used as PrivateDataDir, "/storage/emulated/0/Android/data/org.opencpn.opencpn/files"
                                                // if app has been moved to sdcard, this gives like (on Android 6)
                                                // /storage/2385-1BF8/Android/data/org.opencpn.opencpn/files
        token = tk.GetNextToken();              
        g_androidExtCacheDir = token;
        token = tk.GetNextToken();              
        g_androidExtStorageDir = token;
        
    }
    
    g_mask = -1;
    g_sel = -1;
    
    
    wxStringTokenizer tku(g_androidExtFilesDir, _T("/") );
    while( tku.HasMoreTokens() )
    {
        wxString s1 = tku.GetNextToken();
    
        if(s1.Find(_T("org.")) != wxNOT_FOUND){
            if(s1 != _T("org.opencpn.opencpn") ) g_bstress1 = true;
        }
    }
                
    if(g_bstress1){            
        g_androidUtilHandler->Connect( g_androidUtilHandler->m_stressTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( androidUtilHandler::onStressTimer ), NULL, g_androidUtilHandler );
        g_androidUtilHandler->m_stressTimer.Start(1000, wxTIMER_CONTINUOUS);
    }
    
    return true;
}


wxSize getAndroidConfigSize()
{
    return config_size;
}

void resizeAndroidPersistents()
{
    
     if(g_androidUtilHandler){
         g_androidUtilHandler->m_action = ACTION_RESIZE_PERSISTENTS;
         g_androidUtilHandler->m_eventTimer.Start(100, wxTIMER_ONE_SHOT);
     }
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    java_vm = vm;
    
    // Get JNI Env for all function calls
    if (vm->GetEnv( (void **) &global_jenv, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    
    return JNI_VERSION_1_6;
}

void sendNMEAMessageEvent(wxString &msg)
{
    wxCharBuffer abuf = msg.ToUTF8();
    if( abuf.data() ){                            // OK conversion?
        std::string s(abuf.data());              
//    qDebug() << tstr;
        OCPN_DataStreamEvent Nevent(wxEVT_OCPN_DATASTREAM, 0);
        Nevent.SetNMEAString( s );
        Nevent.SetStream( NULL );
        if(s_pAndroidNMEAMessageConsumer)
            s_pAndroidNMEAMessageConsumer->AddPendingEvent(Nevent);
    }
}
    
    

//      OCPNNativeLib
//      This is a set of methods which can be called from the android activity context.

extern "C"{
JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_test(JNIEnv *env, jobject obj)
{
    //qDebug() << "test";
    
    return 55;
}
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_processSailTimer(JNIEnv *env, jobject obj, double WindAngleMagnetic, double WindSpeedKnots)
    {
        //  The NMEA message target handler may not be setup yet, if no connections are defined or enabled.
        //  But we may want to synthesize messages from the Java app, even without a definite connection, and we want to process these messages too.
        //  So assume that the global MUX, if present, will handle these synthesized messages.
        if( !s_pAndroidNMEAMessageConsumer && g_pMUX ) 
            s_pAndroidNMEAMessageConsumer = g_pMUX;
        
        
        double wind_angle_mag = 0;
        double apparent_wind_angle = 0;
        
        double app_windSpeed = 0;
        double true_windSpeed = 0;
        double true_windDirection = 0;
        
        {
            {
                
                // Need to correct the Magnetic wind angle to True
                // TODO  Punt for mow
                double variation = gVar;
                //qDebug() << "gVar" << gVar;
                
                //  What to use for TRUE ownship head?
                //TODO Look for HDT message contents, if available
                double osHead = gCog;
                bool buseCOG = true;
                //qDebug() << "gHdt" << gHdt;
                
                if( !wxIsNaN(gHdt) ){
                    osHead = gHdt;
                    buseCOG = false;
                }
                
                // What SOG to use?
                double osSog = gSog;
                
                wind_angle_mag = WindAngleMagnetic;
                app_windSpeed = WindSpeedKnots;
                
                // Compute the apparent wind angle
                // If using gCog for ownship head, require speed to be > 0.2 knots
                // If not useing cGog for head, assume we must be using a true heading sensor, so always valid
                if( !wxIsNaN(osHead) && ( (!buseCOG)  ||  (buseCOG && osSog > 0.2) ) ){
                    apparent_wind_angle = wind_angle_mag - (osHead - variation);
                }
                else{
                    apparent_wind_angle = 0;
                }
                if(apparent_wind_angle < 0)
                    apparent_wind_angle += 360.;
                if(apparent_wind_angle > 360.)
                    apparent_wind_angle -= 360.;
                
                
                //  Using the "Law of cosines", compute the true wind speed
                if( !wxIsNaN(osSog) ){
                        true_windSpeed = sqrt( (osSog * osSog) + (app_windSpeed * app_windSpeed) - (2 * osSog * app_windSpeed * cos(apparent_wind_angle * PI / 180.)) );
                }
                else{
                        true_windSpeed = app_windSpeed;
                }
                    
                    // Rearranging the Law of cosines, we calculate True Wind Direction
                if( ( !wxIsNaN(osSog) ) && ( !wxIsNaN(osHead) )  && ( osSog > 0.2)  &&  (true_windSpeed > 1) ){
                        double acosTW = ((osSog * osSog) + (true_windSpeed * true_windSpeed) - (app_windSpeed * app_windSpeed)) / (2 * osSog * true_windSpeed);
                        
                        double twd0 = acos( acosTW) *  ( 180. / PI );
                        
                        // OK on the beat...
                        if(apparent_wind_angle > 180.){
                            true_windDirection = osHead + 180 + twd0;
                        }
                        else{
                            true_windDirection = osHead + 180 - twd0;
                        }                    
                }
                else{
                        true_windDirection = wind_angle_mag + variation;
                }
                    
                if(true_windDirection < 0)
                        true_windDirection += 360.;
                if(true_windDirection > 360.)
                        true_windDirection -= 360.;
                    
                //qDebug() << wind_angle_mag << app_windSpeed << apparent_wind_angle << true_windSpeed << true_windDirection;
                    
                if( s_pAndroidNMEAMessageConsumer ) {
                        
                        NMEA0183        parser;
                    
                        // Now make some NMEA messages
                        // We dont want to pass the incoming MWD message thru directly, since it is not really correct.  The angle is correct, but the speed is relative.
                        //  Make a new MWD sentence with calculated values
                        parser.TalkerID = _T("OS");
                        
                        // MWD
                        SENTENCE sntd;
                        parser.Mwd.WindAngleTrue = true_windDirection; 
                        parser.Mwd.WindAngleMagnetic = wind_angle_mag;
                        parser.Mwd.WindSpeedKnots = true_windSpeed;
                        parser.Mwd.WindSpeedms = true_windSpeed * 0.5144;           // convert kts to m/s
                        parser.Mwd.Write( sntd );
                        sendNMEAMessageEvent(sntd.Sentence);
                        
                        // Now make two MWV sentences
                        // Apparent
                        SENTENCE snt;
                        parser.Mwv.WindAngle = apparent_wind_angle;
                        parser.Mwv.WindSpeed = app_windSpeed;
                        parser.Mwv.WindSpeedUnits = _T("N");
                        parser.Mwv.Reference = _T("R");
                        parser.Mwv.IsDataValid = NTrue;
                        parser.Mwv.Write( snt );
                        sendNMEAMessageEvent(snt.Sentence);
                        
                        // True
                        SENTENCE sntt;
                        double true_relHead = 0;
                        if( !wxIsNaN(osHead) && ( (!buseCOG)  ||  (buseCOG && osSog > 0.2) ) )
                            true_relHead = true_windDirection - osHead;
                        
                        if(true_relHead < 0)
                            true_relHead += 360.;
                        if(true_relHead > 360.)
                            true_relHead -= 360.;
                        
                        parser.Mwv.WindAngle = true_relHead;
                        parser.Mwv.WindSpeed = true_windSpeed;
                        parser.Mwv.WindSpeedUnits = _T("N");
                        parser.Mwv.Reference = _T("T");
                        parser.Mwv.IsDataValid = NTrue;
                        parser.Mwv.Write( sntt );
                        sendNMEAMessageEvent(sntt.Sentence);
                        
                }
            }
        }
        
        return 52;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_processNMEA(JNIEnv *env, jobject obj, jstring nmea_string)
    {
        //  The NMEA message target handler may not be setup yet, if no connections are defined or enabled.
        //  But we may get synthesized messages from the Java app, even without a definite connection, and we want to process these messages too.
        //  So assume that the global MUX, if present, will handle these messages.
        wxEvtHandler  *consumer = s_pAndroidNMEAMessageConsumer;
        
        if( !consumer && g_pMUX ) 
            consumer = g_pMUX;
                
            
        const char *string = env->GetStringUTFChars(nmea_string, NULL);

        //qDebug() << string;
        
        char tstr[200];
        strncpy(tstr, string, 190);
        strcat(tstr, "\r\n");
        
        if( consumer ) {
            OCPN_DataStreamEvent Nevent(wxEVT_OCPN_DATASTREAM, 0);
            Nevent.SetNMEAString( tstr );
            Nevent.SetStream( NULL );
                
            consumer->AddPendingEvent(Nevent);
        }
        
        return 66;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_processBTNMEA(JNIEnv *env, jobject obj, jstring nmea_string)
    {
        const char *string = env->GetStringUTFChars(nmea_string, NULL);
        wxString wstring = wxString(string, wxConvUTF8);
        
        char tstr[200];
        strncpy(tstr, string, 190);
        strcat(tstr, "\r\n");
        
        if( s_pAndroidBTNMEAMessageConsumer ) {
            OCPN_DataStreamEvent Nevent(wxEVT_OCPN_DATASTREAM, 0);
            Nevent.SetNMEAString( tstr );
            Nevent.SetStream( NULL );
            
            s_pAndroidBTNMEAMessageConsumer->AddPendingEvent(Nevent);
        }
        
        return 77;
    }
}


extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onConfigChange(JNIEnv *env, jobject obj)
    {
        qDebug() << "onConfigChange";

        wxLogMessage(_T("onConfigChange"));
        GetAndroidDisplaySize();
        
        wxSize new_size = getAndroidDisplayDimensions();
        
        config_size = new_size;
        
         wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
         evt.SetId( ID_CMD_TRIGGER_RESIZE );
             g_androidUtilHandler->AddPendingEvent(evt);

        return 77;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onMouseWheel(JNIEnv *env, jobject obj, int dir)
    {
        
        wxMouseEvent evt(wxEVT_MOUSEWHEEL);
        evt.m_wheelRotation = dir;
///v5
//         if(cc1 && cc1->GetEventHandler()){
//             cc1->GetEventHandler()->AddPendingEvent(evt);
//         }
        
        return 77;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onMenuKey(JNIEnv *env, jobject obj)
    {

        ///v5gFrame->ToggleToolbar();
            
        return 88;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onStop(JNIEnv *env, jobject obj)
    {
        qDebug() << "onStop";
        wxLogMessage(_T("onStop"));
        
        
        //  App may be summarily killed after this point due to OOM condition.
        //  So we need to persist some dynamic data.
        if(pConfig){
        
        //  Persist the config file, especially to capture the viewport location,scale etc.
            pConfig->UpdateSettings();
        
        //  There may be unsaved objects at this point, and a navobj.xml.changes restore file
        //  We commit the navobj deltas, and flush the restore file 
            pConfig->UpdateNavObj();

        }
        
        g_running = false;

        qDebug() << "onStop return 98";
        return 98;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onStart(JNIEnv *env, jobject obj)
    {
        qDebug() << "onStart";
        wxLogMessage(_T("onStart"));
        
        if(g_bstress1) ShowNavWarning();
        
        g_running = true;

        return 99;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onPause(JNIEnv *env, jobject obj)
    {
        qDebug() << "onPause";
        wxLogMessage(_T("onPause"));
        g_bSleep = true;
        
        if(!g_btrackContinuous)
            androidGPSService( GPS_OFF );
        
        return 97;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onResume(JNIEnv *env, jobject obj)
    {
        qDebug() << "onResume";
        wxLogMessage(_T("onResume"));
        
        int ret = 96;
        
        g_bSleep = false;
        
        if(bGPSEnabled)
            androidGPSService( GPS_ON );
        
#if 0
///v5        
        if(cc1){
            glChartCanvas *glc = cc1->GetglCanvas();
            QGLWidget *glw = glc->GetHandle();
//            qDebug() << glw;
            if(glw){
                 QGLContext *context = glw->context();
//                 qDebug() << context;
                 if(context){
//                     qDebug() << context->isValid();
                     if(context->isValid())
                         ret = 1;
                     else{
                         wxLogMessage(_T("OpenGL Lost Context"));
                         qDebug() << "OpenGL Lost Context";
                         ret = 0;
                     }
 
                 }
            }
        }
#endif        
        wxCommandEvent evt0(wxEVT_COMMAND_MENU_SELECTED);
        evt0.SetId( ID_CMD_CLOSE_ALL_DIALOGS );
        if(gFrame && gFrame->GetEventHandler())
            gFrame->GetEventHandler()->AddPendingEvent(evt0);
        
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
        evt.SetId( ID_CMD_INVALIDATE );
        if(gFrame)
            gFrame->GetEventHandler()->AddPendingEvent(evt);

        return ret;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_onDestroy(JNIEnv *env, jobject obj)
    {
        qDebug() << "onDestroy";
        wxLogMessage(_T("onDestroy"));
        
        if(pConfig){
            //  Persist the config file, especially to capture the viewport location,scale, locale etc.
            pConfig->UpdateSettings();
        }
        
        g_running = false;
        
        return 98;
    }
}

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_selectChartDisplay(JNIEnv *env, jobject obj, int type, int family)
    {
        
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
        if(type == CHART_TYPE_CM93COMP){
            evt.SetId( ID_CMD_SELECT_CHART_TYPE );
            evt.SetExtraLong( CHART_TYPE_CM93COMP);
        }
        else{
            evt.SetId( ID_CMD_SELECT_CHART_FAMILY );
            evt.SetExtraLong( family);
        }
        
        if(gFrame)
            gFrame->GetEventHandler()->AddPendingEvent(evt);
        
        return 74;
    }
}
    
extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_invokeCmdEventCmdString(JNIEnv *env, jobject obj, int cmd_id, jstring s)
    {
         const char *sparm;
        wxString wx_sparm;
        JNIEnv* jenv;
        
        //  Need a Java environment to decode the string parameter
        if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
            //qDebug() << "GetEnv failed.";
        }
        else {
            sparm = (jenv)->GetStringUTFChars(s, NULL);
            wx_sparm = wxString(sparm, wxConvUTF8);
        }
 
        //qDebug() << "invokeCmdEventCmdString" << cmd_id << s;
        
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
        evt.SetId( cmd_id );
        evt.SetString( wx_sparm);
        
        if(gFrame){
            qDebug() << "add event" << cmd_id << wx_sparm.mbc_str();
            gFrame->GetEventHandler()->AddPendingEvent(evt);
        }
        else
            qDebug() << "No frame for EventCmdString";

        
        return 71;
    }
}
    
        
extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_invokeMenuItem(JNIEnv *env, jobject obj, int item)
    {
        if(!gFrame)                     // App Frame not yet set up, on slow devices
            return 71;
        
        wxString msg1;
        msg1.Printf(_T("invokeMenuItem: %d"), item);
        wxLogMessage(msg1);
        
        // If in Route Create, disable all other menu items
         if( gFrame && (gFrame->GetFocusCanvas()->m_routeState > 1 ) && (OCPN_ACTION_ROUTE != item) ) {
             wxLogMessage(_T("invokeMenuItem A"));
             return 72;
         }

        wxLogMessage(_T("invokeMenuItem B"));
        
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
        
        switch(item){
            case OCPN_ACTION_FOLLOW:
                evt.SetId( ID_MENU_NAV_FOLLOW );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_ROUTE:
                evt.SetId( ID_MENU_ROUTE_NEW );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_RMD:
                evt.SetId( ID_MENU_ROUTE_MANAGER );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_SETTINGS_BASIC:
                evt.SetId( ID_MENU_SETTINGS_BASIC );
                wxLogMessage(_T("invokeMenuItem OCPN_ACTION_SETTINGS_BASIC"));
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_TRACK_TOGGLE:
                evt.SetId( ID_MENU_NAV_TRACK );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_MOB:
                evt.SetId( ID_MENU_MARK_MOB );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_TIDES_TOGGLE:
                evt.SetId( ID_MENU_SHOW_TIDES );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_CURRENTS_TOGGLE:
                evt.SetId( ID_MENU_SHOW_CURRENTS );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_ENCTEXT_TOGGLE:
                evt.SetId( ID_MENU_ENC_TEXT );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_ENCSOUNDINGS_TOGGLE:
                evt.SetId( ID_MENU_ENC_SOUNDINGS );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            case OCPN_ACTION_ENCLIGHTS_TOGGLE:
                evt.SetId( ID_MENU_ENC_LIGHTS );
                gFrame->GetEventHandler()->AddPendingEvent(evt);
                break;
                
            default:
                break;
        }
        
        return 73;
    }
}


extern "C"{
    JNIEXPORT jstring JNICALL Java_org_opencpn_OCPNNativeLib_getVPCorners(JNIEnv *env, jobject obj)
    {
//        qDebug() << "getVPCorners";
        
        wxString s;
        
        if(gFrame->GetPrimaryCanvas()){
            LLBBox vbox;
            vbox = gFrame->GetPrimaryCanvas()->GetVP().GetBBox();
            s.Printf(_T("%g;%g;%g;%g;"), vbox.GetMaxLat(), vbox.GetMaxLon(), vbox.GetMinLat(), vbox.GetMinLon());  
        }
                    
        jstring ret = (env)->NewStringUTF(s.c_str());
        
        return ret;
    }
        
}       

extern "C"{
    JNIEXPORT jstring JNICALL Java_org_opencpn_OCPNNativeLib_getVPS(JNIEnv *env, jobject obj)
    {
        wxString s;
        
        if(gFrame->GetPrimaryCanvas()){
            ViewPort vp = gFrame->GetPrimaryCanvas()->GetVP();
            s.Printf(_T("%g;%g;%g;%g;%g;"), vp.clat, vp.clon, vp.view_scale_ppm, gLat, gLon);  
            
        }
        
        jstring ret = (env)->NewStringUTF(s.c_str());
        
        return ret;
    }
    
}       

extern "C"{
    JNIEXPORT int JNICALL Java_org_opencpn_OCPNNativeLib_getTLWCount(JNIEnv *env, jobject obj)
    {
        int ret = 0;
        wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
        while (node)
        {
            wxWindow* win = node->GetData();
            if(win->IsShown())
                ret++;
            
            node = node->GetNext();
        }
        return ret;
    }
}       

extern "C"{
    JNIEXPORT int JNICALL Java_org_opencpn_OCPNNativeLib_notifyFullscreenChange(JNIEnv *env, jobject obj, bool bFull)
    {
        g_bFullscreen = bFull;
        return 1;
    }    
}       


extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_setDownloadStatus(JNIEnv *env, jobject obj, int status, jstring url)
    {
//        qDebug() << "setDownloadStatus";
 
        const char *sparm;
        wxString wx_sparm;
        JNIEnv* jenv;
        
        //  Need a Java environment to decode the string parameter
        if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
            //qDebug() << "GetEnv failed.";
        }
        else {
            sparm = (jenv)->GetStringUTFChars(url, NULL);
            wx_sparm = wxString(sparm, wxConvUTF8);
        }
        
        if(s_bdownloading && wx_sparm.IsSameAs(s_requested_url) ){
            
//            qDebug() << "Maybe mine...";
            //  We simply pass the event on to the core download manager methods,
            //  with parameters crafted to the event
            OCPN_downloadEvent ev(wxEVT_DOWNLOAD_EVENT, 0);
            
            OCPN_DLCondition dl_condition = OCPN_DL_EVENT_TYPE_UNKNOWN;
            OCPN_DLStatus dl_status = OCPN_DL_UNKNOWN;
            
            //  Translate Android status values to OCPN 
            switch (status){
                case 16:                                // STATUS_FAILED
                    dl_condition = OCPN_DL_EVENT_TYPE_END;
                    dl_status = OCPN_DL_FAILED;
                    break;
                    
                case 8:                                 // STATUS_SUCCESSFUL
                    dl_condition = OCPN_DL_EVENT_TYPE_END;
                    dl_status = OCPN_DL_NO_ERROR;
                    break;
                    
                case 4:                                 //  STATUS_PAUSED
                case 2:                                 //  STATUS_RUNNING 
                case 1:                                 //  STATUS_PENDING
                   dl_condition = OCPN_DL_EVENT_TYPE_PROGRESS;
                   dl_status = OCPN_DL_NO_ERROR;
            }
                   
            ev.setDLEventCondition( dl_condition );
            ev.setDLEventStatus( dl_status );
            
            if(s_download_evHandler){
//                qDebug() << "Sending event...";
                s_download_evHandler->AddPendingEvent(ev);
            }
            
            
        }
       
        
        return 77;
    }
    
}       

extern "C"{
    JNIEXPORT jint JNICALL Java_org_opencpn_OCPNNativeLib_sendPluginMessage(JNIEnv *env, jobject obj, jstring msgID, jstring msg)
    {
        const char *sparm;
        wxString MsgID;
        wxString Msg;
        JNIEnv* jenv;
        
        //  Need a Java environment to decode the string parameter
        if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
            //qDebug() << "GetEnv failed.";
        }
        else {
            sparm = (jenv)->GetStringUTFChars(msgID, NULL);
            MsgID = wxString(sparm, wxConvUTF8);
            
            sparm = (jenv)->GetStringUTFChars(msg, NULL);
            Msg = wxString(sparm, wxConvUTF8);
            
        }
        
        SendPluginMessage( MsgID, Msg );
        
        return 74;
    }
}

void androidTerminate(){
    callActivityMethod_vs("terminateApp");
}

bool CheckPendingJNIException()
{
    JNIEnv* jenv;
    
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) 
        return true;

    if( (jenv)->ExceptionCheck() == JNI_TRUE ) {

        // Handle exception here.
        (jenv)->ExceptionDescribe(); // writes to logcat
        (jenv)->ExceptionClear();
        
        return false;           // There was a pending exception, but cleared OK
                                // interesting discussion:  http://blog.httrack.com/blog/2013/08/23/catching-posix-signals-on-android/
    }
    
    return false;
    
}


wxString callActivityMethod_vs(const char *method)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return return_string;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod(method, "()Ljava/lang/String;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    jstring s = data.object<jstring>();
    //qDebug() << s;
    
    if(s){
        //  Need a Java environment to decode the resulting string
        if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
            //qDebug() << "GetEnv failed.";
        }
        else {
            const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
            return_string = wxString(ret_string, wxConvUTF8);
        }
    }
    
    return return_string;
}



wxString callActivityMethod_is(const char *method, int parm)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return return_string;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod(method, "(I)Ljava/lang/String;", parm);
    if(CheckPendingJNIException())
        return _T("NOK");
    
    jstring s = data.object<jstring>();
    
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
    return return_string;
    
}

wxString callActivityMethod_iis(const char *method, int parm1, int parm2)
{
    if(CheckPendingJNIException())
        return _T("NOK");
   
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return return_string;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod(method, "(II)Ljava/lang/String;", parm1, parm2);
    if(CheckPendingJNIException())
        return _T("NOK");
    
    jstring s = data.object<jstring>();
    
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
    return return_string;
    
}

wxString callActivityMethod_ss(const char *method, wxString parm)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return return_string;
    }

    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
        return _T("jenv Error");
    }
    
    jstring p = (jenv)->NewStringUTF(parm.c_str());
    
    
    //  Call the desired method
    //qDebug() << "Calling method_ss";
    //qDebug() << method;
    
    QAndroidJniObject data = activity.callObjectMethod(method, "(Ljava/lang/String;)Ljava/lang/String;", p);
    
    (jenv)->DeleteLocalRef(p);
    
    if(CheckPendingJNIException())
        return _T("NOK");
    
    //qDebug() << "Back from method_ss";
    
    jstring s = data.object<jstring>();
    
    if( (jenv)->GetStringLength( s )){
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
    return return_string;
    
}

wxString callActivityMethod_s2s(const char *method, wxString parm1, wxString parm2)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return return_string;
    }
    
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
        return _T("jenv Error");
    }
    
    wxCharBuffer p1b = parm1.ToUTF8();
    jstring p1 = (jenv)->NewStringUTF(p1b.data());
    
    wxCharBuffer p2b = parm2.ToUTF8();
    jstring p2 = (jenv)->NewStringUTF(p2b.data());
    
    //  Call the desired method
    //qDebug() << "Calling method_s2s" << " (" << method << ")";
    
    QAndroidJniObject data = activity.callObjectMethod(method, "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", p1, p2);

    (jenv)->DeleteLocalRef(p1);
    (jenv)->DeleteLocalRef(p2);
    
    if(CheckPendingJNIException())
        return _T("NOK");
    
    //qDebug() << "Back from method_s2s";
    
    jstring s = data.object<jstring>();
    
    if( (jenv)->GetStringLength( s )){
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
    return return_string;
    
}

wxString callActivityMethod_s3s(const char *method, wxString parm1, wxString parm2, wxString parm3)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        return return_string;
    }
    
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        return _T("jenv Error");
    }
    
    wxCharBuffer p1b = parm1.ToUTF8();
    jstring p1 = (jenv)->NewStringUTF(p1b.data());
    
    wxCharBuffer p2b = parm2.ToUTF8();
    jstring p2 = (jenv)->NewStringUTF(p2b.data());
    
    wxCharBuffer p3b = parm3.ToUTF8();
    jstring p3 = (jenv)->NewStringUTF(p3b.data());
    
    //  Call the desired method
    //qDebug() << "Calling method_s3s" << " (" << method << ")";
    
    QAndroidJniObject data = activity.callObjectMethod(method, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                                                       p1, p2, p3);
    (jenv)->DeleteLocalRef(p1);
    (jenv)->DeleteLocalRef(p2);
    (jenv)->DeleteLocalRef(p3);
    
    if(CheckPendingJNIException())
        return _T("NOK");

    //qDebug() << "Back from method_s3s";
        
    jstring s = data.object<jstring>();
        
    if( (jenv)->GetStringLength( s )){
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
        
    return return_string;
        
}


wxString callActivityMethod_s4s(const char *method, wxString parm1, wxString parm2, wxString parm3, wxString parm4)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    JNIEnv* jenv;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return return_string;
    }
    
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
        return _T("jenv Error");
    }
    
    wxCharBuffer p1b = parm1.ToUTF8();
    jstring p1 = (jenv)->NewStringUTF(p1b.data());
    
    wxCharBuffer p2b = parm2.ToUTF8();
    jstring p2 = (jenv)->NewStringUTF(p2b.data());
    
    wxCharBuffer p3b = parm3.ToUTF8();
    jstring p3 = (jenv)->NewStringUTF(p3b.data());
    
    wxCharBuffer p4b = parm4.ToUTF8();
    jstring p4 = (jenv)->NewStringUTF(p4b.data());

    //const char *ts = (jenv)->GetStringUTFChars(p2, NULL);
    //qDebug() << "Test String p2" << ts;
    
    //  Call the desired method
    //qDebug() << "Calling method_s4s" << " (" << method << ")";
    
    QAndroidJniObject data = activity.callObjectMethod(method, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
                                                       p1, p2, p3, p4);
    (jenv)->DeleteLocalRef(p1);
    (jenv)->DeleteLocalRef(p2);
    (jenv)->DeleteLocalRef(p3);
    (jenv)->DeleteLocalRef(p4);
    
    if(CheckPendingJNIException())
        return _T("NOK");
    
    //qDebug() << "Back from method_s4s";
    
    jstring s = data.object<jstring>();
    
     if( (jenv)->GetStringLength( s )){
         const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
         return_string = wxString(ret_string, wxConvUTF8);
     }
    
    return return_string;
    
}

wxString callActivityMethod_s2s2i(const char *method, wxString parm1, wxString parm2, int parm3, int parm4)
{
    if(CheckPendingJNIException())
        return _T("NOK");
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return _T("NOK");
    
    if ( !activity.isValid() ){
        return return_string;
    }
    
    //  Need a Java environment to decode the resulting string
    JNIEnv* jenv;
    
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
        return _T("jenv Error");
    }
    
    wxCharBuffer p1b = parm1.ToUTF8();
    jstring p1 = (jenv)->NewStringUTF(p1b.data());

    wxCharBuffer p2b = parm2.ToUTF8();
    jstring p2 = (jenv)->NewStringUTF(p2b.data());
    
    //qDebug() << "Calling method_s2s2i" << " (" << method << ")";
    //qDebug() << parm3 << parm4;
    
    QAndroidJniObject data = activity.callObjectMethod(method, "(Ljava/lang/String;Ljava/lang/String;II)Ljava/lang/String;",
                                                       p1, p2, parm3, parm4);
    
    (jenv)->DeleteLocalRef(p1);
    (jenv)->DeleteLocalRef(p2);
    
    if(CheckPendingJNIException())
        return _T("NOK");
    
    jstring s = data.object<jstring>();
        
    if( (jenv)->GetStringLength( s )){
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
             return_string = wxString(ret_string, wxConvUTF8);
     }
        
    return return_string;
        
}


wxString androidGetAndroidSystemLocale()
{
    return callActivityMethod_vs("getAndroidLocaleString");
}   
    
bool androidGetFullscreen()
{
    wxString s = callActivityMethod_vs("getFullscreen");
    
    return s == _T("YES");
}

bool androidSetFullscreen( bool bFull )
{
    callActivityMethod_is("setFullscreen", (int)bFull);
    
    return true;
}

void androidLaunchHelpView()
{
    callActivityMethod_vs("launchHelpView");
}

void androidLaunchBrowser( wxString URL )
{
    callActivityMethod_ss("launchWebView", URL);
}


bool androidShowDisclaimer( wxString title, wxString msg )
{
    if(CheckPendingJNIException())
        return false;
    
    wxString return_string;
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if(CheckPendingJNIException())
        return false;
    
    if ( !activity.isValid() )
        return false;
    
    JNIEnv* jenv;
    
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) 
        return false;
    
    
    wxCharBuffer p1b = title.ToUTF8();
    jstring p1 = (jenv)->NewStringUTF(p1b.data());
    
    // Convert for wxString-UTF8  to jstring-UTF16
    wxWCharBuffer b = msg.wc_str();
    jstring p2 = (jenv)->NewString( (jchar *)b.data(), msg.Len() * 2);
    
    QAndroidJniObject data = activity.callObjectMethod( "disclaimerDialog", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", p1, p2);
    
    (jenv)->DeleteLocalRef(p1);
    (jenv)->DeleteLocalRef(p2);
    
    if(CheckPendingJNIException())
        return false;
        
    jstring s = data.object<jstring>();
        
    if( (jenv)->GetStringLength( s )){
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
        
        
    return (return_string == _T("OK"));
}

extern PlatSpec android_plat_spc;

wxString androidGetDeviceInfo()
{
    if(!g_deviceInfo.Length())
        g_deviceInfo = callActivityMethod_vs("getDeviceInfo");
    
    wxStringTokenizer tkz(g_deviceInfo, _T("\n"));
    while( tkz.HasMoreTokens() )
    {
        wxString s1 = tkz.GetNextToken();
        if(wxNOT_FOUND != s1.Find(_T("OS API Level"))){
            int a = s1.Find(_T("{"));
            if(wxNOT_FOUND != a){
                wxString b = s1.Mid(a+1, 2);
                memset(&android_plat_spc.msdk[0], 0, 3);
                strncpy(&android_plat_spc.msdk[0], b.c_str(), 2);
            }
        }
        if(wxNOT_FOUND != s1.Find(_T("opencpn"))){
            strcpy(&android_plat_spc.hn[0], s1.c_str());
        }
    }
    
    return g_deviceInfo;
}

wxString androidGetHomeDir()
{
    return g_androidFilesDir + _T("/");
}

wxString androidGetPrivateDir()                 // Used for logfile, config file, navobj, and the like
{
    if(g_bExternalApp){
        
        // should check storage availability
#if 0
/* Checks if external storage is available for read and write */
        public boolean isExternalStorageWritable() {
            String state = Environment.getExternalStorageState();
            if (Environment.MEDIA_MOUNTED.equals(state)) {
                return true;
            }
            return false;
        }
        
        /* Checks if external storage is available to at least read */
        public boolean isExternalStorageReadable() {
            String state = Environment.getExternalStorageState();
            if (Environment.MEDIA_MOUNTED.equals(state) ||
                Environment.MEDIA_MOUNTED_READ_ONLY.equals(state)) {
                return true;
                }
                return false;
        }
#endif        
        return g_androidExtFilesDir;
    }

    // We choose to use the ExtFiles directory always , so that the contents of logfiles, navobjs, etc.
    // may always be accessible by simple Android File Explorers...
    return g_androidExtFilesDir;    
}

wxString androidGetSharedDir()                 // Used for assets like uidata, s57data, etc
{
//    if(g_bExternalApp){
//        if(g_androidExtFilesDir.Length())
//            return g_androidExtFilesDir + _T("/");
//    }
    
    return g_androidExtFilesDir + _T("/");
}

wxString androidGetCacheDir()                 // Used for raster_texture_cache, mmsitoname.csv, etc
{
//    if(g_bExternalApp){
//        if(g_androidExtCacheDir.Length())
//            return g_androidExtCacheDir;
//    }
    
    return g_androidExtCacheDir;
}

// Android notes:
/* Note: don't be confused by the word "external" here.
 * This directory can better be thought as media/shared storage.
 * It is a filesystem that can hold a relatively large amount of data
 * and that is shared across all applications (does not enforce permissions).
 * Traditionally this is an SD card, but it may also be implemented as built-in storage
 * in a device that is distinct from the protected internal storage
 * and can be mounted as a filesystem on a computer.
 */

wxString androidGetExtStorageDir()                 // Used for Chart storage, typically
{
    return g_androidExtStorageDir;
}

extern void androidSetRouteAnnunciator(bool viz)
{
    callActivityMethod_is("setRouteAnnunciator", viz?1:0);
}

extern void androidSetFollowTool(bool bactive)
{
//    qDebug() << "setFollowIconState" << bactive;
    
    if(g_follow_active != bactive)
        callActivityMethod_is("setFollowIconState", bactive?1:0);
    
    g_follow_active = bactive;
}

extern void androidSetTrackTool(bool bactive)
{
    if(g_track_active != bactive)
        callActivityMethod_is("setTrackIconState", bactive?1:0);
    
    g_track_active = bactive;
}


void androidSetChartTypeMaskSel( int mask, wxString &indicator)
{
    int sel = 0;
    if(wxNOT_FOUND != indicator.Find( _T("raster")))
        sel = 1;
    else if(wxNOT_FOUND != indicator.Find( _T("vector")))
        sel = 2;
    else if(wxNOT_FOUND != indicator.Find( _T("cm93")))
        sel = 4;

    if((g_mask != mask) || (g_sel != sel)){
//        qDebug() << "androidSetChartTypeMaskSel" << mask << sel;
        callActivityMethod_iis("configureNavSpinnerTS", mask, sel);
        g_mask = mask;
        g_sel = sel;
    }
}       


void androidEnableBackButton(bool benable)
{
    callActivityMethod_is("setBackButtonState", benable?1:0);
    g_backEnabled = benable;
}

void androidEnableBackButtonCheck(bool benable)
{
    if(g_backEnabled != benable)
        androidEnableBackButton(benable);
}


bool androidGetMemoryStatus( int *mem_total, int *mem_used )
{
    
    //  On android, We arbitrarily declare that we have used 50% of available memory.
    if(mem_total)
        *mem_total = s_androidMemTotal * 1024;
    if(mem_used)
        *mem_used = s_androidMemUsed * 1024;
    return true;
    
#if 0
    
    //  Get a reference to the running native activity
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        qDebug() << "Activity is not valid";
        return false;
    }

    unsigned long android_processID = wxGetProcessId();
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("getMemInfo", "(I)Ljava/lang/String;", (int)android_processID);
    
//    wxString return_string;
    jstring s = data.object<jstring>();
    
    int mu = 50;
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        mu = atoi(ret_string);
        
    }
    
    if(mem_used)
        *mem_used = mu;

        
    return true;
#endif    
}

double GetAndroidDisplaySize()
{

    double ret = 200.;          // sane default
    
    //  Get a reference to the running native activity
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return false;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("getDisplayMetrics", "()Ljava/lang/String;");
    
    wxString return_string;
    jstring s = data.object<jstring>();
    
    JNIEnv* jenv;
    //  Need a Java environment to decode the resulting string
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
    //  Return string may have commas instead of periods, if using Euro locale
    //  We just fix it here...
    return_string.Replace( _T(","), _T(".") );

    wxSize screen_size = wxSize(1,2);
    
    if(QApplication::desktop()){                // Desktop might not yet be initialized
        screen_size = ::wxGetDisplaySize();
    }
        
    ///v5wxLogMessage(_T("Metrics:") + return_string);
    wxString msg;
    
    msg.Printf(_T("wxGetDisplaySize(): %d %d"), screen_size.x, screen_size.y);
    ///v5wxLogMessage(msg);
    
    double density = 1.0;
    long androidWidth = 2;
    long androidHeight = 1;
    long androidDmWidth = 2;
    long androidDmHeight = 1;
    long abh;
    
    wxStringTokenizer tk(return_string, _T(";"));
    if( tk.HasMoreTokens() ){
        wxString token = tk.GetNextToken();     // xdpi
        token = tk.GetNextToken();              // density
        
        token.ToDouble( &density );

        token = tk.GetNextToken();              // ldpi
        
        token = tk.GetNextToken();              // width
        token.ToLong( &androidWidth );
        token = tk.GetNextToken();              // height - statusBarHeight
        token = tk.GetNextToken();              // width
        token = tk.GetNextToken();              // height
        token.ToLong( &androidHeight );
        
        token = tk.GetNextToken();              // dm.widthPixels
        token.ToLong( &androidDmWidth );
        token = tk.GetNextToken();              // dm.heightPixels
        token.ToLong( &androidDmHeight );
        
        token = tk.GetNextToken();              // actionBarHeight
        token.ToLong( &abh );

    }
    
    double ldpi = 160. * density;
    if(ldpi < 160)
        ldpi = 160.;
    
    // Find the max dimension among all possibilities
    double maxDim = wxMax(screen_size.x, screen_size.y);
    maxDim = wxMax(maxDim, androidHeight);
    maxDim = wxMax(maxDim, androidWidth);
    
    ret = (maxDim / ldpi) * 25.4;

    if(ret < 75){               // 3 inches is too small....
        double ret_bad = ret;
        ret = 100;
        msg.Printf(_T("WARNING: Android Auto Display Size OVERRIDE_TOO_SMALL: %g  ldpi: %g  density: %g correctedsize: %g "), ret_bad, ldpi, density, ret);
    }
    else if(ret > 400){         // Too large
        double ret_bad = ret;
        ret = 400;
        msg.Printf(_T("WARNING: Android Auto Display Size OVERRIDE_TOO_LARGE: %g  ldpi: %g  density: %g corrected size: %g"), ret_bad, ldpi, density, ret);
    }
    else{        
        msg.Printf(_T("Android Auto Display Size (mm, est.): %g   ldpi: %g  density: %g"), ret, ldpi, density);
    }
    ///v5wxLogMessage(msg);
    
    //  Save some items as global statics for convenience
    g_androidDPmm = ldpi / 25.4;
    g_androidDensity = density;
    g_ActionBarHeight = wxMax(abh, 50);
    
    //qDebug() << "GetAndroidDisplaySize" << ldpi << g_androidDPmm;

    return ret;
}

int getAndroidActionBarHeight()
{
    return g_ActionBarHeight;
}

double getAndroidDPmm()
{
    // Returns an estimate based on the pixel density reported
    ///v5if( g_androidDPmm < 0.01){
        GetAndroidDisplaySize();
    //}
    
    //qDebug() << "getAndroidDPmm" << g_androidDPmm;
    
    // User override?
    if(g_config_display_size_manual && (g_config_display_size_mm > 0) ){
        double maxDim = wxMax(::wxGetDisplaySize().x, ::wxGetDisplaySize().y);
        double size_mm = g_config_display_size_mm;
        size_mm = wxMax(size_mm, 50);
        size_mm = wxMin(size_mm, 400);
        double ret = maxDim / size_mm;
        qDebug() << "getAndroidDPmm override" << maxDim << size_mm << g_config_display_size_mm;
        return ret;
    }
        
        
    if(g_androidDPmm > 0.01)
        return g_androidDPmm;
    else
        return 160. / 25.4;
}

double getAndroidDisplayDensity()
{
    if( g_androidDensity < 0.01){
        GetAndroidDisplaySize();
    }
    
//    qDebug() << "g_androidDensity" << g_androidDensity;
    
    if(g_androidDensity > 0.01)
        return g_androidDensity;
    else
        return 1.0;
}
    

wxSize getAndroidDisplayDimensions( void )
{
    wxSize sz_ret = ::wxGetDisplaySize();               // default, probably reasonable, but maybe not accurate
    
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return sz_ret;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("getDisplayMetrics", "()Ljava/lang/String;");
    
    wxString return_string;
    jstring s = data.object<jstring>();
    
    //  Need a Java environment to decode the resulting string
    JNIEnv* jenv;
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
    //167.802994;1.000000;160;1024;527;1024;552;1024;552;56
     wxStringTokenizer tk(return_string, _T(";"));
    if( tk.HasMoreTokens() ){
        wxString token = tk.GetNextToken();     // xdpi
        token = tk.GetNextToken();              // density
        token = tk.GetNextToken();              // densityDPI
        
        token = tk.GetNextToken();
        long a = ::wxGetDisplaySize().x;        // default is wxWidgets idea
        if(token.ToLong( &a ))
            sz_ret.x = a;
        
        token = tk.GetNextToken();
        long b = ::wxGetDisplaySize().y;        
        if(token.ToLong( &b ))
            sz_ret.y = b;
        
        token = tk.GetNextToken();              
        token = tk.GetNextToken();
        
        token = tk.GetNextToken();
        token = tk.GetNextToken();
        
        long abh = 0;
        token = tk.GetNextToken();              //  ActionBar height, if shown
        if(token.ToLong( &abh ))
            sz_ret.y -= abh;
            
        
        
    }

//    qDebug() << sz_ret.x << sz_ret.y;
    
    return sz_ret;
    
}

void androidConfirmSizeCorrection()
{
    //  There is some confusion about the ActionBar size during configuration changes.
    //  We need to confirm the calculated display size, and fix it if necessary.
    //  This happens during staged resize events 
 
    wxLogMessage(_T("androidConfirmSizeCorrection"));
    wxSize targetSize = getAndroidDisplayDimensions();
    qDebug() << "Confirming" << targetSize.y << config_size.y;
    if(config_size != targetSize){
        qDebug() << "Correcting";
        gFrame->SetSize(targetSize);
        config_size = targetSize;
    }
}
        
void androidForceFullRepaint()
{
    
        wxLogMessage(_T("androidForceFullRepaint"));
        wxSize targetSize = getAndroidDisplayDimensions();
        wxSize tempSize = targetSize;
        tempSize.y--;
        gFrame->SetSize(tempSize);
//        gFrame->SetSize(targetSize);
       
        GetAndroidDisplaySize();
        
        wxSize new_size = getAndroidDisplayDimensions();
        config_size = new_size;
        
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
        evt.SetId( ID_CMD_TRIGGER_RESIZE );
        if(gFrame && gFrame->GetEventHandler()){
            gFrame->GetEventHandler()->AddPendingEvent(evt);
        }
        
}       

void androidShowBusyIcon()
{
    if(b_androidBusyShown)
        return;

    //qDebug() << "ShowBusy";
        
    //  Get a reference to the running native activity
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("showBusyCircle", "()Ljava/lang/String;");
    
    b_androidBusyShown = true;
}

void androidHideBusyIcon()
{
    if(!b_androidBusyShown)
        return;
    
    //  Get a reference to the running native activity
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return;
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("hideBusyCircle", "()Ljava/lang/String;");

    b_androidBusyShown = false;
}


//---------------------------------------------------------------
//      GPS Device Support
//---------------------------------------------------------------
bool androidDeviceHasGPS()
{
    wxString query = androidGPSService( GPS_PROVIDER_AVAILABLE );
    wxLogMessage( query);
    
    bool result = query.Upper().IsSameAs(_T("YES"));
    if(result){
        //qDebug() << "Android Device has internal GPS";
        wxLogMessage(_T("Android Device has internal GPS"));
    }
    else{
        //qDebug() << "Android Device has NO internal GPS";
        wxLogMessage(_T("Android Device has NO internal GPS"));
    }
    return result;
}

bool androidStartNMEA(wxEvtHandler *consumer)
{
    s_pAndroidNMEAMessageConsumer = consumer;

    //qDebug() << "androidStartNMEA";
    wxString s;
    
    s = androidGPSService( GPS_ON );
    wxLogMessage(s);
    if(s.Upper().Find(_T("DISABLED")) != wxNOT_FOUND){
        OCPNMessageBox(NULL,
                       _("Your android device has an internal GPS, but it is disabled.\n\
                       Please visit android Settings/Location dialog to enable GPS"),
                        _T("OpenCPN"), wxOK );        
        
        androidStopNMEA();
        return false;
    }
    else
        bGPSEnabled = true;
    
    return true;
}

bool androidStopNMEA()
{
    s_pAndroidNMEAMessageConsumer = NULL;
    
    wxString s = androidGPSService( GPS_OFF );
    
    bGPSEnabled = false;
    
    return true;
}


wxString androidGPSService(int parm)
{
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return _T("Activity is not valid");
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("queryGPSServer", "(I)Ljava/lang/String;", parm);
    
    wxString return_string;
    jstring s = data.object<jstring>();
    
    //  Need a Java environment to decode the resulting string
    JNIEnv* jenv;
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        return_string = wxString(ret_string, wxConvUTF8);
    }
    
     return return_string;
}


bool androidDeviceHasBlueTooth()
{
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
                                                                           "activity", "()Landroid/app/Activity;");
    
    if ( !activity.isValid() ){
        //qDebug() << "Activity is not valid";
        return _T("Activity is not valid");
    }
    
    //  Call the desired method
    QAndroidJniObject data = activity.callObjectMethod("hasBluetooth", "(I)Ljava/lang/String;", 0);
    
    wxString query;
    jstring s = data.object<jstring>();
    
    //  Need a Java environment to decode the resulting string
    JNIEnv* jenv;
    if (java_vm->GetEnv( (void **) &jenv, JNI_VERSION_1_6) != JNI_OK) {
        //qDebug() << "GetEnv failed.";
    }
    else {
        const char *ret_string = (jenv)->GetStringUTFChars(s, NULL);
        query = wxString(ret_string, wxConvUTF8);
    }
    
    bool result = query.Upper().IsSameAs(_T("YES"));
    
    if(result){
        //qDebug() << "Android Device has internal Bluetooth";
        wxLogMessage(_T("Android Device has internal Bluetooth"));
    }
    else{
        //qDebug() << "Android Device has NO internal Bluetooth";
        wxLogMessage(_T("Android Device has NO internal Bluetooth"));
    }
    
    return result;
}

bool androidStartBluetoothScan()
{
    wxString result = callActivityMethod_is("startBlueToothScan", 0);
    
    return true;
    
}

bool androidStopBluetoothScan()
{
    wxString result = callActivityMethod_is("stopBlueToothScan", 0);
    
    return true;
    
}

bool androidStartBT(wxEvtHandler *consumer, wxString mac_address )
{
    s_pAndroidBTNMEAMessageConsumer = consumer;
    
    if(mac_address.Find(':') ==  wxNOT_FOUND)   //  does not look like a mac address
        return false;
    
    wxString result = callActivityMethod_ss("startBTService", mac_address);
    
    return true;
}
    
bool androidStopBT()
{
    //qDebug() << "androidStopBT";
    
    s_pAndroidBTNMEAMessageConsumer = NULL;
    
    wxString result = callActivityMethod_is("stopBTService", 0);
        
    return true;
}
    
    
wxArrayString androidGetBluetoothScanResults()
{
    wxArrayString ret_array;

    wxString result = callActivityMethod_is("getBlueToothScanResults", 0);
    
    wxStringTokenizer tk(result, _T(";"));
    while ( tk.HasMoreTokens() )
    {
        wxString token = tk.GetNextToken();
        ret_array.Add(token);
    }
    
    if(!ret_array.GetCount())
        ret_array.Add(_("Nothing found"));
    
    return ret_array;
}

bool androidCheckOnline()
{
    wxString val = callActivityMethod_vs("isNetworkAvailable");
    return val.IsSameAs(_T("YES"));
}

wxArrayString *androidGetSerialPortsArray( void )
{
  
    wxArrayString *pret_array = new wxArrayString;
    wxString result = callActivityMethod_is("scanSerialPorts", 0);
    
    wxStringTokenizer tk(result, _T(";"));
    while ( tk.HasMoreTokens() )
    {
        wxString token = tk.GetNextToken();
        pret_array->Add(token);
    }
    
    return pret_array;
}

bool androidStartUSBSerial(wxString &portname, wxString& baudRate, wxEvtHandler *consumer)
{
    wxString result = callActivityMethod_s2s("startSerialPort", portname, baudRate);
    
    s_pAndroidNMEAMessageConsumer = consumer;
    
    return true;
}

bool androidStopUSBSerial(wxString &portname)
{
    s_pAndroidNMEAMessageConsumer = NULL;
    
    //  If app is closing down, the USB serial ports will go away automatically.
    //  So no need here.
    //  In fact, stopSerialPort() causes an occasional error when closing app.
    //  Dunno why, difficult to debug.
    if(!b_inCloseWindow)
        wxString result = callActivityMethod_ss("stopSerialPort", portname);
    
    return true;
}



int androidFileChooser( wxString *result, const wxString &initDir, const wxString &title,
                        const wxString &suggestion, const wxString &wildcard, bool dirOnly, bool addFile)
{
    wxString tresult;
    
    //  Start a timer to poll for results 
    if(g_androidUtilHandler){
        g_androidUtilHandler->m_eventTimer.Stop();
        g_androidUtilHandler->m_done = false;

        wxString activityResult;
        if(dirOnly)
            activityResult = callActivityMethod_s2s2i("DirChooserDialog", initDir, title, addFile, 0);
        
        else
            activityResult = callActivityMethod_s4s("FileChooserDialog", initDir, title, suggestion, wildcard);
        
        if(activityResult == _T("OK") ){
            //qDebug() << "ResultOK, starting spin loop";
            g_androidUtilHandler->m_action = ACTION_FILECHOOSER_END;
            g_androidUtilHandler->m_eventTimer.Start(1000, wxTIMER_CONTINUOUS);
        
        //  Spin, waiting for result
            while(!g_androidUtilHandler->m_done){
                wxMilliSleep(50);
                wxSafeYield(NULL, true);
            }
        
            //qDebug() << "out of spin loop";
            g_androidUtilHandler->m_action = ACTION_NONE;
            g_androidUtilHandler->m_eventTimer.Stop();
        
        
            tresult = g_androidUtilHandler->GetStringResult();
            
            if( tresult.StartsWith(_T("cancel:")) ){
                //qDebug() << "Cancel1";
                return wxID_CANCEL;
            }
            else if( tresult.StartsWith(_T("file:")) ){
                if(result){
                    *result = tresult.AfterFirst(':');
                    //qDebug() << "OK";
                    return wxID_OK;
                }
                else{
                    //qDebug() << "Cancel2";
                    return wxID_CANCEL;
                }
            }
        }
        else{
            //qDebug() << "Result NOT OK";
        }
        
    }

    return wxID_CANCEL;
}

    

bool InvokeJNIPreferences( wxString &initial_settings)
{
    bool ret = true;
    wxCharBuffer abuf = initial_settings.ToUTF8();
    if( !abuf.data() )
        return false;
    
    //  Create the method parameter(s)
        QAndroidJniObject param1 = QAndroidJniObject::fromString(abuf.data());
        
        //  Get a reference to the running native activity
        QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",
        "activity", "()Landroid/app/Activity;");
        
        if ( !activity.isValid() ){
            //qDebug() << "Activity is not valid";
            return false;
        }
        
        //  Call the desired method
        activity.callObjectMethod("doAndroidSettings", "(Ljava/lang/String;)Ljava/lang/String;", param1.object<jstring>());
        
        
        return ret;
}

wxString BuildAndroidSettingsString( void )
{
    wxString result;

    //  Start with chart dirs
    if( ChartData){
        wxArrayString chart_dir_array = ChartData->GetChartDirArrayString();

        for(unsigned int i=0 ; i < chart_dir_array.GetCount() ; i++){
            result += _T("ChartDir:");
            result += chart_dir_array[i];
            result += _T(";");
        }
    }

    //  Now the simple Boolean parameters
        result += _T("prefb_lookahead:") + wxString(g_bLookAhead == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_quilt:") + wxString(g_bQuiltEnable == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_showgrid:") + wxString(g_bDisplayGrid == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_showoutlines:") + wxString(g_bShowOutlines == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_showdepthunits:") + wxString(g_bShowDepthUnits == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_lockwp:") + wxString(g_bWayPointPreventDragging == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_confirmdelete:") + wxString(g_bConfirmObjectDelete == 1 ? _T("1;") : _T("0;"));
        result += _T("prefb_expertmode:") + wxString(g_bUIexpert == 1 ? _T("1;") : _T("0;"));

#ifdef USE_S57        
        if(ps52plib){
            result += _T("prefb_showlightldesc:") + wxString(ps52plib->m_bShowLdisText == 1 ? _T("1;") : _T("0;"));
            result += _T("prefb_showimptext:") + wxString(ps52plib->m_bShowS57ImportantTextOnly == 1 ? _T("1;") : _T("0;"));
            result += _T("prefb_showSCAMIN:") + wxString(ps52plib->m_bUseSCAMIN == 1 ? _T("1;") : _T("0;"));
            result += _T("prefb_showsound:") + wxString(ps52plib->m_bShowSoundg == 1 ? _T("1;") : _T("0;"));
            result += _T("prefb_showATONLabels:") + wxString(ps52plib->m_bShowAtonText == 1 ? _T("1;") : _T("0;"));
        }
#endif        
    // Some other assorted values
        result += _T("prefs_navmode:") + wxString(g_bCourseUp == 0 ? _T("North Up;") : _T("Course Up;"));
        result += _T("prefs_chartInitDir:") + *pInit_Chart_Dir + _T(";");

        wxString s;
        double sf = (g_GUIScaleFactor * 10) + 50.;
        s.Printf( _T("%3.0f;"), sf );
        s.Trim(false);
        result += _T("prefs_UIScaleFactor:") + s;
        
        sf = (g_ChartScaleFactor * 10) + 50.;
        s.Printf( _T("%3.0f;"), sf );
        s.Trim(false);
        result += _T("prefs_chartScaleFactor:") + s;
        
#ifdef USE_S57        
        if(ps52plib){
            wxString nset = _T("Base");
            switch( ps52plib->GetDisplayCategory() ){
                case ( DISPLAYBASE ):
                    nset = _T("Base;");
                    break;
                case ( STANDARD ):
                    nset = _T("Standard;");
                    break;
                case ( OTHER ):
                    nset = _T("All;");
                    break;
                case ( MARINERS_STANDARD ):
                    nset = _T("Mariner Standard;");
                    break;
                default:
                    nset = _T("Base;");
                    break;
            }
            result += _T("prefs_displaycategory:") + nset;
            
    
            if( ps52plib->m_nSymbolStyle == PAPER_CHART )
                nset = _T("Paper Chart;");
            else
                nset = _T("Simplified;");
            result += _T("prefs_vectorgraphicsstyle:") + nset;
            
            if( ps52plib->m_nBoundaryStyle == PLAIN_BOUNDARIES )
                nset = _T("Plain;");
            else
                nset = _T("Symbolized;");
            result += _T("prefs_vectorboundarystyle:") + nset;
            
            if( S52_getMarinerParam( S52_MAR_TWO_SHADES ) == 1.0 )
                nset = _T("2;");
            else
                nset = _T("4;");
            result += _T("prefs_vectorchartcolors:") + nset;
            
            // depth unit conversion factor
          
            float conv = 1;
            int depthUnit = ps52plib->m_nDepthUnitDisplay;
            if ( depthUnit == 0 ) // feet
                conv = 0.3048f; // international definiton of 1 foot is 0.3048 metres
                else if ( depthUnit == 2 ) // fathoms
                conv = 0.3048f * 6; // 1 fathom is 6 feet
                
            s.Printf( _T("%4.2f;"), S52_getMarinerParam( S52_MAR_SHALLOW_CONTOUR ) / conv );
            s.Trim(false);
            result += _T("prefs_shallowdepth:") + s;
            
            s.Printf( _T("%4.2f;"), S52_getMarinerParam( S52_MAR_SAFETY_CONTOUR ) / conv );
            s.Trim(false);
            result += _T("prefs_safetydepth:") + s;
            
            s.Printf( _T("%4.2f;"), S52_getMarinerParam( S52_MAR_DEEP_CONTOUR ) / conv );
            s.Trim(false);
            result += _T("prefs_deepdepth:") + s;
    
            //  Scale slider range from -5 -- 5 in OCPN options.
            //  On Android, the range is 0 -- 100
            //  So, convert
        }
#else
            result += _T("prefs_displaycategory:"); result += _T("Base;");
            result += _T("prefs_vectorgraphicsstyle:");  result += _T("Paper Chart;");
            result += _T("prefs_vectorboundarystyle:");  result += _T("Plain;");
            result += _T("prefs_vectorchartcolors:");  result += _T("2;");
            
            // depth unit conversion factor
            
            float conv = 1;
            int depthUnit = 0;
            if ( depthUnit == 0 ) // feet
                conv = 0.3048f; // international definiton of 1 foot is 0.3048 metres
            else if ( depthUnit == 2 ) // fathoms
                conv = 0.3048f * 6; // 1 fathom is 6 feet
                
            s.Printf( _T("%4.0f;"), 6.0);
            s.Trim(false);
            result += _T("prefs_shallowdepth:");   result += s;
            
            s.Printf( _T("%4.0f;"), 8.0 );
            s.Trim(false);
            result += _T("prefs_safetydepth:");   result += s;
            
            s.Printf( _T("%4.0f;"), 12.0 );
            s.Trim(false);
            result += _T("prefs_deepdepth:");   result += s;

#endif            
        
        // Connections
        
        // Internal GPS.
        for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
        {
            ConnectionParams *cp = g_pConnectionParams->Item(i);
            if(INTERNAL_GPS == cp->Type){
                result += _T("prefb_internalGPS:");
                result += cp->bEnabled ? _T("1;") : _T("0;");
            }                    
            if(SERIAL == cp->Type){
                if(wxNOT_FOUND != cp->GetPortStr().Find(_T("PL2303"))){
                    result += _T("prefb_PL2303:");
                    result += cp->bEnabled ? _T("1;") : _T("0;");
                }
                else if(wxNOT_FOUND != cp->GetPortStr().Find(_T("dAISy"))){
                    result += _T("prefb_dAISy:");
                    result += cp->bEnabled ? _T("1;") : _T("0;");
                }
                else if(wxNOT_FOUND != cp->GetPortStr().Find(_T("FT232R"))){
                    result += _T("prefb_FT232R:");
                    result += cp->bEnabled ? _T("1;") : _T("0;");
                }
                else if(wxNOT_FOUND != cp->GetPortStr().Find(_T("FT231X"))){
                    result += _T("prefb_FT231X:");
                    result += cp->bEnabled ? _T("1;") : _T("0;");
                }
                else if(wxNOT_FOUND != cp->GetPortStr().Find(_T("USBDP"))){
                    result += _T("prefb_USBDP:");
                    result += cp->bEnabled ? _T("1;") : _T("0;");
                }
            }                    
        }
    
    wxLogMessage(_T("BuildAndroidSettingsString: ") + result);
    
    return result;
}

const wxString AUSBNames[] = { _T("AUSBSerial:Prolific_PL2303"), _T("AUSBSerial:FTDI_FT232R"), _T("AUSBSerial:FTDI_FT231X"), _T("AUSBSerial:dAISy"),
                _T("AUSBSerial:USBDP"), _T("LASTENTRY") };
const wxString AUSBPrefs[] = { _T("prefb_PL2303"),               _T("prefb_FT232R"),           _T("prefb_FT231X"),           _T("prefb_dAISy"),
                _T("prefb_USBDP"),         _T("LASTENTRY") };


int androidApplySettingsString( wxString settings, ArrayOfCDI *pACDI)
{
    
    //  Parse the passed settings string
    bool bproc_InternalGPS = false;
    bool benable_InternalGPS = false;
    
    int rr = GENERIC_CHANGED;
    
    // extract chart directories
        
    if(ChartData){
        wxStringTokenizer tkd(settings, _T(";"));
        while ( tkd.HasMoreTokens() ){
            wxString token = tkd.GetNextToken();
            
            if(token.StartsWith( _T("ChartDir"))){
                wxString dir = token.AfterFirst(':');
                if(dir.Length()){
                    ChartDirInfo cdi;
                    cdi.fullpath = dir.Trim();
                    cdi.magic_number = ChartData->GetMagicNumberCached(dir.Trim());
                    pACDI->Add(cdi);
                }
            }
        }
        
        // Scan for changes
        if(!ChartData->CompareChartDirArray( *pACDI )){
            rr |= VISIT_CHARTS;
            rr |= CHANGE_CHARTS;
            wxLogMessage(_T("Chart Dir List change detected"));
        }
    }
    
    
    wxStringTokenizer tk(settings, _T(";"));
    while ( tk.HasMoreTokens() )
    {
        wxString token = tk.GetNextToken();
        wxString val = token.AfterFirst(':');
        
        //  Binary switches
        
        if(token.StartsWith( _T("prefb_lookahead"))){
            g_bLookAhead = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_quilt"))){
            g_bQuiltEnable = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_lockwp"))){
            g_bWayPointPreventDragging = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_showdepthunits"))){
            g_bShowDepthUnits = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_confirmdelete"))){
            g_bConfirmObjectDelete = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_showgrid"))){
            g_bDisplayGrid = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_showoutlines"))){
            g_bShowOutlines = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_expertmode"))){
            g_bUIexpert = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefb_internalGPS"))){
            bproc_InternalGPS = true;
            benable_InternalGPS = val.IsSameAs(_T("1"));
        }
        else if(token.StartsWith( _T("prefs_navmode"))){
            g_bCourseUp = val.IsSameAs(_T("Course Up"));
        }
        else if(token.StartsWith( _T("prefb_trackOnPause"))){
            g_btrackContinuous = val.IsSameAs(_T("1"));
        }
        
        
        //  Strings, etc.
        
        else if(token.StartsWith( _T("prefs_UIScaleFactor"))){
            double a;
            if(val.ToDouble(&a))
                g_GUIScaleFactor = wxRound( (a / 10.) - 5.);
        }
        
        else if(token.StartsWith( _T("prefs_chartScaleFactor"))){
            double a;
            if(val.ToDouble(&a)){
                g_ChartScaleFactor = wxRound( (a / 10.) - 5.);
                g_ChartScaleFactorExp = g_Platform->getChartScaleFactorExp( g_ChartScaleFactor );
            }
        }
        
        else if(token.StartsWith( _T("prefs_chartInitDir"))){
            *pInit_Chart_Dir = val;
        }
        
        #ifdef USE_S57
        if(ps52plib){
            float conv = 1;
            int depthUnit = ps52plib->m_nDepthUnitDisplay;
            if ( depthUnit == 0 ) // feet
                conv = 0.3048f; // international definiton of 1 foot is 0.3048 metres
                else if ( depthUnit == 2 ) // fathoms
                conv = 0.3048f * 6; // 1 fathom is 6 feet
                
                if(token.StartsWith( _T("prefb_showsound"))){
                    bool old_val = ps52plib->m_bShowSoundg;
                    ps52plib->m_bShowSoundg = val.IsSameAs(_T("1"));
                    if(old_val != ps52plib->m_bShowSoundg)
                        rr |= S52_CHANGED;
                }
                else if(token.StartsWith( _T("prefb_showSCAMIN"))){
                    bool old_val = ps52plib->m_bUseSCAMIN;
                    ps52plib->m_bUseSCAMIN = val.IsSameAs(_T("1"));
                    if(old_val != ps52plib->m_bUseSCAMIN)
                        rr |= S52_CHANGED;
                }
                else if(token.StartsWith( _T("prefb_showimptext"))){
                    bool old_val = ps52plib->m_bShowS57ImportantTextOnly;
                    ps52plib->m_bShowS57ImportantTextOnly = val.IsSameAs(_T("1"));
                    if(old_val != ps52plib->m_bShowS57ImportantTextOnly)
                        rr |= S52_CHANGED;
                }
                else if(token.StartsWith( _T("prefb_showlightldesc"))){
                    bool old_val = ps52plib->m_bShowLdisText;
                    ps52plib->m_bShowLdisText = val.IsSameAs(_T("1"));
                    if(old_val != ps52plib->m_bShowLdisText)
                        rr |= S52_CHANGED;
                }
                else if(token.StartsWith( _T("prefb_showATONLabels"))){
                    bool old_val = ps52plib->m_bShowAtonText;
                    ps52plib->m_bShowAtonText = val.IsSameAs(_T("1"));
                    if(old_val != ps52plib->m_bShowAtonText)
                        rr |= S52_CHANGED;
                }
                
                else if(token.StartsWith( _T("prefs_displaycategory"))){
                    _DisCat old_nset = ps52plib->GetDisplayCategory();
                    
                    _DisCat nset = DISPLAYBASE;
                    if(wxNOT_FOUND != val.Lower().Find(_T("base")))
                        nset = DISPLAYBASE;
                    else if(wxNOT_FOUND != val.Lower().Find(_T("mariner")))
                        nset = MARINERS_STANDARD;
                    else if(wxNOT_FOUND != val.Lower().Find(_T("standard")))
                        nset = STANDARD;
                    else if(wxNOT_FOUND != val.Lower().Find(_T("all")))
                        nset = OTHER;
                    
                    if(nset != old_nset){
                        rr |= S52_CHANGED;
                        ps52plib-> SetDisplayCategory( nset );
                    }
                }
                
                else if(token.StartsWith( _T("prefs_shallowdepth"))){
                    double old_dval = S52_getMarinerParam( S52_MAR_SHALLOW_CONTOUR );
                    double dval;
                    if(val.ToDouble(&dval)){
                        if(fabs(dval - old_dval) > .001){
                            S52_setMarinerParam( S52_MAR_SHALLOW_CONTOUR, dval * conv );
                            rr |= S52_CHANGED;
                        }
                    }
                }
                
                else if(token.StartsWith( _T("prefs_safetydepth"))){
                    double old_dval = S52_getMarinerParam( S52_MAR_SAFETY_CONTOUR );
                    double dval;
                    if(val.ToDouble(&dval)){
                        if(fabs(dval - old_dval) > .001){
                            S52_setMarinerParam( S52_MAR_SAFETY_CONTOUR, dval * conv );
                            rr |= S52_CHANGED;
                        }
                    }
                }
                
                else if(token.StartsWith( _T("prefs_deepdepth"))){
                    double old_dval = S52_getMarinerParam( S52_MAR_DEEP_CONTOUR );
                    double dval;
                    if(val.ToDouble(&dval)){
                        if(fabs(dval - old_dval) > .001){
                            S52_setMarinerParam( S52_MAR_DEEP_CONTOUR, dval * conv );
                            rr |= S52_CHANGED;
                        }
                    }
                }
                
                else if(token.StartsWith( _T("prefs_vectorgraphicsstyle"))){
                    LUPname old_LUP = ps52plib->m_nSymbolStyle;
                    
                    if(wxNOT_FOUND != val.Lower().Find(_T("paper")))
                        ps52plib->m_nSymbolStyle = PAPER_CHART;
                    else if(wxNOT_FOUND != val.Lower().Find(_T("simplified")))
                        ps52plib->m_nSymbolStyle = SIMPLIFIED;
                    
                    if(old_LUP != ps52plib->m_nSymbolStyle)
                        rr |= S52_CHANGED;
                    
                }
                
                else if(token.StartsWith( _T("prefs_vectorboundarystyle"))){
                    LUPname old_LUP = ps52plib->m_nBoundaryStyle;
                    
                    if(wxNOT_FOUND != val.Lower().Find(_T("plain")))
                        ps52plib->m_nBoundaryStyle = PLAIN_BOUNDARIES;
                    else if(wxNOT_FOUND != val.Lower().Find(_T("symbolized")))
                        ps52plib->m_nBoundaryStyle = SYMBOLIZED_BOUNDARIES;
                    
                    if(old_LUP != ps52plib->m_nBoundaryStyle)
                        rr |= S52_CHANGED;
                    
                }
                
                else if(token.StartsWith( _T("prefs_vectorchartcolors"))){
                    double old_dval = S52_getMarinerParam( S52_MAR_TWO_SHADES );
                    
                    if(wxNOT_FOUND != val.Lower().Find(_T("2")))
                        S52_setMarinerParam( S52_MAR_TWO_SHADES, 1. );
                    else if(wxNOT_FOUND != val.Lower().Find(_T("4")))
                        S52_setMarinerParam( S52_MAR_TWO_SHADES, 0. );
                    
                    double new_dval = S52_getMarinerParam( S52_MAR_TWO_SHADES );
                    if(fabs(new_dval - old_dval) > .1){
                        rr |= S52_CHANGED;
                    }
                }
        }
        #endif        
    }
    
    // Process Internal GPS Connection
    if(g_pConnectionParams && bproc_InternalGPS){
        
        //  Does the connection already exist?
        ConnectionParams *pExistingParams = NULL;
        ConnectionParams *cp = NULL;
        
        for ( size_t i = 0; i < g_pConnectionParams->Count(); i++ )
        {
            ConnectionParams *xcp = g_pConnectionParams->Item(i);
            if(INTERNAL_GPS == xcp->Type){
                pExistingParams = xcp;
                cp = xcp;
                break;
            }
        }
        
        bool b_action = true;
        if(pExistingParams){
            if(pExistingParams->bEnabled == benable_InternalGPS)
                b_action = false;                    // nothing to do...
                else
                    cp->bEnabled = benable_InternalGPS;
        }
        else if(benable_InternalGPS){           //  Need a new Params
            // make a generic config string for InternalGPS.
            wxString sGPS = _T("2;3;;0;0;;0;1;0;0;;0;;1;0;0;0;0");          // 17 parms
            ConnectionParams *new_params = new ConnectionParams(sGPS);
            
            new_params->bEnabled = benable_InternalGPS;
            g_pConnectionParams->Add(new_params);
            cp = new_params;
        }
        
        
        if(b_action && cp){                               // something to do?

            // Terminate and remove any existing stream with the same port name
            DataStream *pds_existing = g_pMUX->FindStream( cp->GetDSPort() );
            if(pds_existing)
                g_pMUX->StopAndRemoveStream( pds_existing );
            
            
            if( cp->bEnabled ) {
                dsPortType port_type = cp->IOSelect;
                DataStream *dstr = new DataStream( g_pMUX,
                                                   cp->Type,
                                                   cp->GetDSPort(),
                                                   wxString::Format(wxT("%i"), cp->Baudrate),
                                                                    port_type,
                                                                    cp->Priority,
                                                                    cp->Garmin);
                                                   dstr->SetInputFilter(cp->InputSentenceList);
                                                   dstr->SetInputFilterType(cp->InputSentenceListType);
                                                   dstr->SetOutputFilter(cp->OutputSentenceList);
                                                   dstr->SetOutputFilterType(cp->OutputSentenceListType);
                                                   dstr->SetChecksumCheck(cp->ChecksumCheck);
                                                   
                                                   g_pMUX->AddStream(dstr);
                                                   
                                                   cp->b_IsSetup = true;
            }
        }
    }
    
    // Process USB Serial Connections
    bool b_newGlobalSettings = false;
    if(g_pConnectionParams){
        
        int i = 0;
        while( wxNOT_FOUND == AUSBPrefs[i].Find(_T("LASTENTRY")) ){
            wxStringTokenizer tk(settings, _T(";"));
            while ( tk.HasMoreTokens() )
            {
                wxString token = tk.GetNextToken();
                wxString pref = token.BeforeFirst(':');
                wxString val = token.AfterFirst(':');
                wxString extraString;
                
                
                bool benabled = false;
                
                if(pref.IsSameAs(AUSBPrefs[i])){

                    wxLogMessage(_T("pref: ") + pref);
                    wxLogMessage(_T("val: ") + val);
                    
                    if(pref.Contains(_T("USBDP"))){
                        extraString = val.AfterFirst(':');
                        wxLogMessage(_T("extra: ") + extraString);
                    }
                            
                    wxLogMessage(_T("found pref ") + pref);
                    
                    //  Does the connection already exist?
                    ConnectionParams *pExistingParams = NULL;
                    ConnectionParams *cp = NULL;
                    
                    wxString target = AUSBNames[i] + _T("-") + extraString;
                    
                    for ( unsigned int j = 0; j < g_pConnectionParams->Count(); j++ )
                    {
                        ConnectionParams *xcp = g_pConnectionParams->Item(j);
                        wxLogMessage( _T("    Checking: ") + target + " .. " +xcp->GetDSPort());
                        
                        if( (SERIAL == xcp->Type) && (target.IsSameAs(xcp->GetDSPort().AfterFirst(':'))) ){
                            pExistingParams = xcp;
                            cp = xcp;
                            benabled = val.BeforeFirst(':').IsSameAs(_T("1"));
                            break;
                        }
                    }
                    
                    
                    bool b_action = true;
                    if(pExistingParams){
                        wxLogMessage(_T("Using existing connection  ") + target);
                        
                        if(pExistingParams->bEnabled == benabled){
                            b_action = false;                    // nothing to do...
                        }
                        else
                            cp->bEnabled = benabled;
                    }
                    else if(val.BeforeFirst(':').IsSameAs(_T("1"))){           //  Need a new Params
                        // make a generic config string.
                        //0;1;;0;0;/dev/ttyS0;4800;1;0;0;;0;;1;0;0;0;0        17 parms
                        
                        wxString sSerial = _T("0;1;;0;0;");
                        sSerial += AUSBNames[i];
                        sSerial += _T("-") + extraString;
                        sSerial += _T(";4800;1;0;0;;0;;1;0;0;0;0");
                        
                        wxLogMessage(_T("Adding connection  ") + sSerial);
                        
                        ConnectionParams *new_params = new ConnectionParams(sSerial);
                        
                        new_params->bEnabled = true;
                        g_pConnectionParams->Add(new_params);
                        cp = new_params;
                        rr |= NEED_NEW_OPTIONS;
                    }
                    
                    
                    
                    
                    if(b_action && cp){                               // something to do?
                        rr |= NEED_NEW_OPTIONS;
                        
                        // Terminate and remove any existing stream with the same port name
                        DataStream *pds_existing = g_pMUX->FindStream( cp->GetDSPort() );
                        if(pds_existing)
                            g_pMUX->StopAndRemoveStream( pds_existing );
                        
                        
                        if( cp->bEnabled ) {
                            dsPortType port_type = cp->IOSelect;
                            DataStream *dstr = new DataStream( g_pMUX,
                                                               cp->Type,
                                                               cp->GetDSPort(),
                                                               wxString::Format(wxT("%i"), cp->Baudrate),
                                                               port_type,
                                                               cp->Priority,
                                                               cp->Garmin);
                            dstr->SetInputFilter(cp->InputSentenceList);
                            dstr->SetInputFilterType(cp->InputSentenceListType);
                            dstr->SetOutputFilter(cp->OutputSentenceList);
                            dstr->SetOutputFilterType(cp->OutputSentenceListType);
                            dstr->SetChecksumCheck(cp->ChecksumCheck);
                            
                            g_pMUX->AddStream(dstr);
                            
                            cp->b_IsSetup = true;
                        }
                    }
                }
            }   // found pref
            
            i++;
        }       // while
    }
    
    return rr;
}
    
    

bool DoAndroidPreferences( void )
{
    wxLogMessage(_T("Start DoAndroidPreferences"));
    
    wxString settings = BuildAndroidSettingsString();

    wxLogMessage(_T("Call InvokeJNIPreferences"));
    InvokeJNIPreferences(settings);

    return true;
}

wxString doAndroidPOST( const wxString &url, wxString &parms, int timeoutMsec)
{
    //  Start a timer to poll for results 
    if(g_androidUtilHandler){
        g_androidUtilHandler->m_eventTimer.Stop();
        g_androidUtilHandler->m_done = false;

        androidShowBusyIcon();
        
        wxString stimeout;
        stimeout.Printf(_T("%d"), timeoutMsec);
        wxString result = callActivityMethod_s3s( "doHttpPostAsync", url, parms, stimeout );
        
        if(result == _T("OK") ){
            qDebug() << "doHttpPostAsync ResultOK, starting spin loop";
            g_androidUtilHandler->m_action = ACTION_POSTASYNC_END;
            g_androidUtilHandler->m_eventTimer.Start(500, wxTIMER_CONTINUOUS);
            
            //  Spin, waiting for result
            while(!g_androidUtilHandler->m_done){
                wxMilliSleep(50);
                wxSafeYield(NULL, true);
            }
            
            qDebug() << "out of spin loop";
            g_androidUtilHandler->m_action = ACTION_NONE;
            g_androidUtilHandler->m_eventTimer.Stop();
            androidHideBusyIcon();
            
            wxString presult = g_androidUtilHandler->GetStringResult();
            
            return presult;
        }
        else{
            qDebug() << "doHttpPostAsync Result NOT OK";
            androidHideBusyIcon();
        }
    }
        
    return wxEmptyString;    
}
    
    
int startAndroidFileDownload( const wxString &url, const wxString& destination, wxEvtHandler *evh, long *dl_id )
{
//    if(evh)
    {
        s_bdownloading = true;
        s_requested_url = url;
        s_download_evHandler = evh;
    
        wxString result = callActivityMethod_s2s( "downloadFile", url, destination );

        androidShowBusyIcon();
        
        if( result.IsSameAs(_T("NOK")) )
            return 1;                       // general error
            
  //      wxLogMessage(_T("downloads2s result: ") + result);
        long dl_ID;
        wxStringTokenizer tk(result, _T(";"));
        if( tk.HasMoreTokens() ){
            wxString token = tk.GetNextToken();
            if(token.IsSameAs(_T("OK"))){
                token = tk.GetNextToken();
                token.ToLong(&dl_ID);
                *dl_id = dl_ID;
  //              qDebug() << dl_ID;
                return 0;
            }
        }
    }
    
    return -1;
}

int queryAndroidFileDownload( long dl_ID, wxString *result )
{
//    qDebug() << dl_ID;
    
    wxString stat = callActivityMethod_is( "getDownloadStatus", (int)dl_ID );
    if(result)
        *result = stat;
    
//    wxLogMessage( _T("queryAndroidFileDownload: ") + stat); 
    
    if( stat.IsSameAs(_T("NOK")) )
        return 1;                       // general error
    else
        return 0;
    
}

void finishAndroidFileDownload( void )
{
    s_bdownloading = false;
    s_requested_url.Clear();
    s_download_evHandler = NULL;
    androidHideBusyIcon();
    
    return;
}


void cancelAndroidFileDownload( long dl_ID )
{
    wxString stat = callActivityMethod_is( "cancelDownload", (int)dl_ID );
}


bool AndroidUnzip(wxString& zipFile, wxString& destDir, int nStrip, bool bRemoveZip)
{
    wxString ns;
    ns.Printf(_T("%d"), nStrip);

    wxString br;
    br.Printf(_T("%d"), bRemoveZip);
    
    wxString stat = callActivityMethod_s4s( "unzipFile", zipFile, destDir, ns, br  );
    
    if(wxNOT_FOUND == stat.Find(_T("OK")))
        return false;
    
    qDebug() << "unzip start";
    
    bool bDone = false;
    while (!bDone){
        wxMilliSleep(1000);
        wxSafeYield(NULL, true);
        
        qDebug() << "unzip poll";
        
        wxString result = callActivityMethod_ss( "getUnzipStatus", _T("") );
        if(wxNOT_FOUND != result.Find(_T("DONE")))
            bDone = true;
    }
    qDebug() << "unzip done";
    
    return true;    
    
}

    
wxString getFontQtStylesheet(wxFont *font)
{
    // wxString classes = _T("QLabel, QPushButton, QTreeWidget, QTreeWidgetItem, QCheckBox");
    wxString classes = _T("QWidget ");
    
    wxString qstyle = classes + _T("{  font-family: ") + font->GetFaceName() + _T(";font-style: ");
    switch(font->GetStyle()){
        case wxFONTSTYLE_ITALIC:
            qstyle += _T("italic;");
            break;
        case wxFONTSTYLE_NORMAL:
        default:
            qstyle += _T("normal;");
            break;
    }
    qstyle += _T("font-weight: ");
    switch(font->GetWeight()){
        case wxFONTWEIGHT_BOLD:
            qstyle += _T("bold;");
            break;
        case wxFONTWEIGHT_LIGHT:
            qstyle += _T("light;");
            break;
        case wxFONTWEIGHT_NORMAL:
        default:
            qstyle += _T("normal;");
            break;
    }
    
    qstyle += _T("font-size: ");
    wxString fontSize;
    fontSize.Printf(_T("%dpt }"), font->GetPointSize());
    qstyle += fontSize;

    //  Oddity here....
    //  If this line is active, this particular style is applied to ListCtrl() in PlugIns,
    //  But not TreeCtrl.....
    //  ????
    //qstyle += _T("QTreeWidget::item{ border-color:red; border-style:outset; border-width:2px; color:black; }");
    
    return qstyle;
    
}

    

bool androidPlaySound( wxString soundfile )
{
    //qDebug() << "androidPlay";
    
    wxString result = callActivityMethod_ss("playSound", soundfile);
    
    return true;
}
    

wxString androidGetSupplementalLicense( void )
{
    wxString result = callActivityMethod_vs("getGMAPILicense");
    
    result += AndroidSuppLicense;
    
    return result;
}




wxArrayString androidTraverseDir( wxString dir, wxString filespec)
{
    wxArrayString result;
    if(strncmp(android_plat_spc.msdk, "17", 2))   // skip unless running Android 4.2.2, especially Samsung...
        return result;
    
    wxString ir = callActivityMethod_s2s("getAllFilesWithFilespec", dir, filespec);
    
    wxStringTokenizer tk(ir, _T(";"));
    while( tk.HasMoreTokens() ){
        result.Add(tk.GetNextToken());
    }
    
    return result;
}
        
void androidEnableOptionsMenu( bool bEnable )
{
    callActivityMethod_is("enableOptionsMenu", bEnable?1:0);
}

        
//    Android specific style sheet management

//  ------------Runtime modified globals
QString qtStyleSheetDialog;
QString qtStyleSheetListBook;
QString qtStyleSheetScrollbars;

//--------------Stylesheet prototypes

//  Generic dialog stylesheet
//  Typically adjusted at runtime for display density

QString qtStyleSheetDialogProto = "\
QSlider::groove\
{\
    border: 1px solid #999999;\
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E6E6E6, stop:1 #EEEEEE);\
}\
QSlider::groove:disabled\
{\
    background: #efefef;\
}\
\
QSlider::handle\
{\
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7cb0e9, stop:1 #7cb0e9);\
    border: 1px solid #5c5c5c;\
    border-radius: 3px;\
    width: 80px;\
    height: 45px;\
}\
\
QSlider::handle:disabled\
{\
    background: #D3D0CD;\
}\
\
QScrollBar:horizontal {\
    border: 0px solid grey;\
    background-color: transparent;\
    height: 4px;\
    margin: 0px 1px 0 1px;\
}\
QScrollBar::handle:horizontal {\
    background-color: rgb(150, 150, 150);\
    min-width: 20px;\
}\
QScrollBar::add-line:horizontal {\
    border: 0px solid grey;\
    background: transparent;\
    width: 1px;\
    subcontrol-position: right;\
    subcontrol-origin: margin;\
}\
\
QScrollBar::sub-line:horizontal {\
    border: 0px solid grey;\
    background: transparent;\
    width: 1px;\
    subcontrol-position: left;\
    subcontrol-origin: margin;\
}\
\
QScrollBar:vertical {\
    border: 0px solid grey;\
    background-color: transparent;\
    width: 4px;\
    margin: 1px 0px 1px 0px;\
}\
QScrollBar::handle:vertical {\
    background-color: rgb(150, 150, 150);\
    min-height: 20px;\
}\
QScrollBar::add-line:vertical {\
    border: 0px solid grey;\
    background: transparent;\
    height: 1px;\
    subcontrol-position: top;\
    subcontrol-origin: margin;\
}\
\
QScrollBar::sub-line:vertical {\
    border: 0px solid grey;\
    background: transparent;\
    height: 1px;\
    subcontrol-position: bottom;\
    subcontrol-origin: margin;\
}\
\
QTreeWidget QScrollBar:vertical {\
    border: 0px solid grey;\
    background-color: rgb(240, 240, 240);\
    width: 35px;\
    margin: 1px 0px 1px 0px;\
}\
QTreeWidget QScrollBar::handle:vertical {\
    background-color: rgb(200, 200, 200);\
    min-height: 20px;\
    border-radius: 10px;\
}\
QTreeWidget QScrollBar::add-line:vertical {\
    border: 0px solid grey;\
    background: #32CC99;\
    height: 0px;\
    subcontrol-position: top;\
    subcontrol-origin: margin;\
}\
\
QTreeWidget QScrollBar::sub-line:vertical {\
    border: 0px solid grey;\
    background: #32CC99;\
    height: 0px;\
    subcontrol-position: bottom;\
    subcontrol-origin: margin;\
}\
\
QTreeWidget QScrollBar:horizontal {\
    border: 0px solid grey;\
    background-color: rgb(240, 240, 240);\
    height: 35px;\
    margin: 0px 1px 0 1px;\
}\
QTreeWidget QScrollBar::handle:horizontal {\
    background-color: rgb(200, 200, 200);\
    min-width: 20px;\
    border-radius: 10px;\
}\
QTreeWidget QScrollBar::add-line:horizontal {\
    border: 0px solid grey;\
    background: #32CC99;\
    width: 0px;\
    subcontrol-position: right;\
    subcontrol-origin: margin;\
}\
\
QTreeWidget QScrollBar::sub-line:horizontal {\
    border: 0px solid grey;\
    background: #32CC99;\
    width: 0px;\
    subcontrol-position: left;\
    subcontrol-origin: margin;\
}\
\
#OCPNCheckedListCtrl QScrollBar::vertical {\
    border: 0px solid grey;\
    background-color: rgb(240, 240, 240);\
    width: 45px;\
    margin: 1px 0px 1px 0px;\
}\
#OCPNCheckedListCtrl QScrollBar::handle:vertical {\
    background-color: rgb(180, 180, 180);\
    min-height: 45px;\
    border-radius: 6px;\
}\
#OCPNCheckedListCtrl QScrollBar::add-line:vertical {\
    border: 0px solid grey;\
    background: #32CC99;\
    height: 0px;\
    subcontrol-position: top;\
    subcontrol-origin: margin;\
}\
\
#OCPNCheckedListCtrl QScrollBar::sub-line:vertical {\
    border: 0px solid grey;\
    background: #32CC99;\
    height: 0px;\
    subcontrol-position: bottom;\
    subcontrol-origin: margin;\
}";



QString qtStyleSheetScrollbarsProto ="\
QScrollBar:horizontal {\
    border: 0px solid grey;\
    background-color: transparent;\
    height: 35px;\
    margin: 0px 1px 0 1px;\
}\
QScrollBar::handle:horizontal {\
    background-color: #7cb0e9;\
    min-width: 20px;\
}\
QScrollBar::add-line:horizontal {\
    border: 0px solid grey;\
    background: transparent;\
    width: 1px;\
    subcontrol-position: right;\
    subcontrol-origin: margin;\
}\
\
QScrollBar::sub-line:horizontal {\
    border: 0px solid grey;\
    background: transparent;\
    width: 1px;\
    subcontrol-position: left;\
    subcontrol-origin: margin;\
}\
\
QScrollBar:vertical {\
    border: 0px solid grey;\
    background-color: transparent;\
    width: 35px;\
    margin: 1px 0px 1px 0px;\
}\
QScrollBar::handle:vertical {\
    background-color: #7cb0e9;\
    min-height: 20px;\
}\
QScrollBar::add-line:vertical {\
    border: 0px solid grey;\
    background: transparent;\
    height: 1px;\
    subcontrol-position: top;\
    subcontrol-origin: margin;\
}\
\
QScrollBar::sub-line:vertical {\
    border: 0px solid grey;\
    background: transparent;\
    height: 1px;\
    subcontrol-position: bottom;\
    subcontrol-origin: margin;\
}";



std::string prepareStyleIcon( wxString icon_file, int size )
{
    wxString data_locn = g_Platform->GetSharedDataDir();
    data_locn.Append( _T("styles/") );

    wxString file = data_locn + icon_file;
    
    wxImage Image(file, wxBITMAP_TYPE_PNG);
    wxImage scaledImage = Image.Scale( size, size, wxIMAGE_QUALITY_HIGH );
    
    wxString save_file = g_Platform->GetPrivateDataDir() + _T("/") + icon_file;
    scaledImage.SaveFile(save_file, wxBITMAP_TYPE_PNG);

    wxCharBuffer buf = save_file.ToUTF8();
    std::string ret(buf);
    return ret;
}
    
QString prepareAndroidSliderStyleSheet(int sliderWidth)
{
    QString qtStyleSheetSlider;
    
    
    //  Create and fix up the qtStyleSheetDialog for generic dialog
    
    // adjust the Slider specification
    
    int slider_handle_width = wxMax(g_Platform->GetDisplayDPmm() * 6, sliderWidth / 5);
    
    char sb[600];
    snprintf(sb, sizeof(sb),
    "QSlider::groove { border: 1px solid #999999;  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E6E6E6, stop:1 #EEEEEE); } \
    QSlider::groove:disabled { background: #efefef; } \
    QSlider::handle { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7cb0e9, stop:1 #7cb0e9); border: 1px solid #5c5c5c; \
    border-radius: 3px; width: %dpx; height: 45px; } \
    QSlider::handle:disabled { background: #D3D0CD;}", slider_handle_width);
    
    qtStyleSheetSlider.append(sb);
    
    return qtStyleSheetSlider;
}

    
void prepareAndroidStyleSheets()
{
    
    //  Create and fix up the qtStyleSheetDialog for generic dialog
    qtStyleSheetDialog.clear();
    qtStyleSheetDialog.append(qtStyleSheetDialogProto);
    
    // add the Slider specification
    
    int slider_handle_width = g_Platform->GetDisplayDPmm() * 6;
    
    char sb[400];
    snprintf(sb, sizeof(sb),
    "QSlider::groove { border: 1px solid #999999;  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E6E6E6, stop:1 #EEEEEE); } \
    QSlider::groove:disabled { background: #efefef; } \
    QSlider::handle { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #7cb0e9, stop:1 #7cb0e9); border: 1px solid #5c5c5c; \
    border-radius: 3px; width: %dpx; height: 45px; } \
    QSlider::handle:disabled { background: #D3D0CD;}", slider_handle_width);
    
    qtStyleSheetDialog.append(sb);
    
    
    
    // add the checkbox specification
    int cbSize = 30 * getAndroidDisplayDensity();
    char cb[400];
    
    // icons
     // Checked box
    std::string cbs = prepareStyleIcon(_T("chek_full.png"), cbSize);
    //  Empty box
    std::string ucbs = prepareStyleIcon(_T("chek_empty.png"), cbSize);
    
    snprintf(cb, sizeof(cb), "QCheckBox { spacing: 25px;}\
    QCheckBox::indicator { width: %dpx;   height: %dpx;}\
    QCheckBox::indicator:checked {image: url(%s);}\
    QCheckBox::indicator:unchecked {image: url(%s);}", cbSize, cbSize, cbs.c_str(), ucbs.c_str());
    
    qtStyleSheetDialog.append(cb);

    
    //   The qTabBar buttons as in a listbook
    qtStyleSheetListBook.clear();
    
    // compute the tabbar button size
    int tbbSize = 50 * getAndroidDisplayDensity();
    char tbb[400];
    

    std::string tbbl = prepareStyleIcon(_T("tabbar_button_left.png"), tbbSize);
    std::string tbbr = prepareStyleIcon(_T("tabbar_button_right.png"), tbbSize);
    
    snprintf(tbb, sizeof(tbb), "QTabBar::scroller { width: %dpx; }\
    QTabBar QToolButton::right-arrow { image: url(%s); }\
    QTabBar QToolButton::left-arrow { image: url(%s); }", tbbSize, tbbr.c_str(), tbbl.c_str());
    
    qtStyleSheetListBook.append(tbb);

    
    // A simple stylesheet with scrollbars only
    qtStyleSheetScrollbars.clear();
    qtStyleSheetScrollbars.append(qtStyleSheetScrollbarsProto);
}    
    
void setChoiceStyleSheet( wxChoice *win, int refDim)
{
    //qDebug() << "refDim" << refDim;
    
    float fontDimFloat = ((float)refDim) * 0.5;
    int fontDim = (int)fontDimFloat;
    int pixRadius = refDim / 4;
    
    QString styleString;
    char sb[1400];
    
    
     //  This one control the appearance of the "un-dropped" control.
    snprintf(sb, sizeof(sb),
             "QComboBox { font-size: %dpx; font-weight: bold; min-height: %dpx; color: rgb(0,0,0); background-color: rgb(250,250,250); }", fontDim, refDim );
    styleString.append(sb);
    
    // This one controls the color and style of the drop list items
    snprintf(sb, sizeof(sb),
             "QComboBox QListView::item { color: rgb(0,0,0); background-color: rgb(95, 163, 237); }");
    styleString.append(sb);
    
    
    // This one controls the drop list font
    snprintf(sb, sizeof(sb),
             "QComboBox QAbstractItemView { font-size: %dpx; font-weight: bold;}", fontDim);
    styleString.append(sb);
    
    // This one is necessary to set min height of drop list items, otherwise they are squished.
    snprintf(sb, sizeof(sb),
             "QComboBox QAbstractItemView::item {  min-height: %dpx; border: 10px outset darkgray; border-radius: %dpx;  }", refDim, pixRadius);
    styleString.append(sb); 
    
    //qDebug() << styleString;
    
    win->GetHandle()->setView(new QListView());         // Magic
    win->GetHandle()->setStyleSheet(styleString);
 
    
}


void setMenuStyleSheet( wxMenu *win, const wxFont& font)
{
    if(!win)
        return;
    
    int points = font.GetPointSize();
    int fontPix = points / g_Platform->getFontPointsperPixel();
     
    //qDebug() << points << g_Platform->getFontPointsperPixel() << fontPix;
    
    QString styleString;
    char sb[1400];
    
    snprintf(sb, sizeof(sb),
             "QMenu { font: bold %dpx; }", fontPix );
    styleString.append(sb);

    snprintf(sb, sizeof(sb),
             "QMenu::separator { height: 4px; background: lightblue; margin-left: 10px; margin-right: 5px; }");
    styleString.append(sb);
    
    
    //qDebug() << styleString;
    
    win->GetHandle()->setStyleSheet(styleString);
}


QString getAdjustedDialogStyleSheet()
{
    return qtStyleSheetDialog;
}

QString getListBookStyleSheet()
{
    return qtStyleSheetListBook;
}

QString getScrollBarsStyleSheet()
{
    return qtStyleSheetScrollbars;
}


//      SVG Support
wxBitmap loadAndroidSVG( const wxString filename, unsigned int width, unsigned int height )
{
    wxCharBuffer abuf = filename.ToUTF8();
    if( abuf.data() ){                            // OK conversion?
        std::string s(abuf.data());              
        //qDebug() << "loadAndroidSVG" << s.c_str();
    }
    else{
        qDebug() << "loadAndroidSVG FAIL";
    }        
    
    // Destination file location
    wxString save_file_dir = g_Platform->GetPrivateDataDir() + _T("/") + _T("icons");
    if( !wxDirExists(save_file_dir) )
        wxMkdir( save_file_dir);
    
    wxFileName fsvg(filename);
    wxFileName fn(save_file_dir + _T("/") + fsvg.GetFullName());
    fn.SetExt(_T("png"));

/*
       //Caching does not work well, since we always build each icon twice.
    if(fn.FileExists()){
        wxBitmap bmp_test(fn.GetFullPath(), wxBITMAP_TYPE_PNG);
        if(bmp_test.IsOk()){
            if((bmp_test.GetWidth() == (int)width) && (bmp_test.GetHeight() == (int)height))
                return bmp_test;
        }
    }
*/            
        
    wxString val = callActivityMethod_s2s2i("buildSVGIcon", filename, fn.GetFullPath(), width, height);
    if( val == _T("OK") ){
//        qDebug() << "OK";
        
        return wxBitmap(fn.GetFullPath(), wxBITMAP_TYPE_PNG);
    }
    else{
        return wxBitmap(width, height);
    }
}

void androidTestCPP()
{
    callActivityMethod_vs("callFromCpp");
}

unsigned int androidColorPicker( unsigned int initialColor)
{
    if(g_androidUtilHandler){
        g_androidUtilHandler->m_eventTimer.Stop();
        g_androidUtilHandler->m_done = false;
        
        wxString val = callActivityMethod_is("doColorPickerDialog", initialColor);
    
   
        if(val == _T("OK") ){
            //qDebug() << "ResultOK, starting spin loop";
            g_androidUtilHandler->m_action = ACTION_COLORDIALOG_END;
            g_androidUtilHandler->m_eventTimer.Start(1000, wxTIMER_CONTINUOUS);
            
            //  Spin, waiting for result
            while(!g_androidUtilHandler->m_done){
                wxMilliSleep(50);
                wxSafeYield(NULL, true);
            }
            
            //qDebug() << "out of spin loop";
            g_androidUtilHandler->m_action = ACTION_NONE;
            g_androidUtilHandler->m_eventTimer.Stop();
            
            
            wxString tresult = g_androidUtilHandler->GetStringResult();
            
            if( tresult.StartsWith(_T("cancel:")) ){
                //qDebug() << "Cancel1";
                return initialColor;
            }
            else if( tresult.StartsWith(_T("color:")) ){
                wxString color = tresult.AfterFirst(':');
                long a;
                color.ToLong(&a);
                unsigned int b = a;
                
                //char cc[30];
                //sprintf(cc, "%0X", b);
                //qDebug() << "OK " << cc;
                
                return b;
            }
        }
        else{
            qDebug() << "Result NOT OK";
        }
    }
    return 0;
}










