HM_DOM = (document.getElementById) ? true : false;
HM_NS4 = (document.layers) ? true : false;
HM_IE = (document.all) ? true : false;
HM_IE4 = HM_IE && !HM_DOM;
HM_Mac = (navigator.appVersion.indexOf("Mac") != -1);
HM_IE4M = HM_IE4 && HM_Mac;
HM_IsMenu = (HM_DOM || HM_NS4 || (HM_IE4 && !HM_IE4M));

HM_BrowserString = HM_NS4 ? "NS4" : HM_DOM ? "DOM" : "IE4";

if(window.event + "" == "undefined") event = null;
function HM_f_PopUp(){return false};
function HM_f_PopDown(){return false};
popUp = HM_f_PopUp;
popDown = HM_f_PopDown;


HM_GL_MenuWidth          = 150;
HM_GL_FontFamily         = "Arial,sans-serif";
HM_GL_FontSize           = 8;
HM_GL_FontBold           = false;
HM_GL_FontItalic         = false;
HM_GL_FontColor          = "#000066";
HM_GL_FontColorOver      = "#000000";
HM_GL_BGColor            = "#FFFFFF";
HM_GL_BGColorOver        = "#CCCCFF";
HM_GL_ItemPadding        = 3;

HM_GL_BorderWidth        = 1;
HM_GL_BorderColor        = "#000066";
HM_GL_BorderStyle        = "solid";
HM_GL_SeparatorSize      = 1;
HM_GL_SeparatorColor     = "#000066";
HM_GL_ImageSrc           = "dscript79/tri.gif";
HM_GL_ImageSrcLeft       = "dscript79/triL.gif";
HM_GL_ImageSize          = 16;
HM_GL_ImageHorizSpace    = 1;
HM_GL_ImageVertSpace     = 2;

HM_GL_KeepHilite         = false;
HM_GL_ClickStart         = false;
HM_GL_ClickKill          = 0;
HM_GL_ChildOverlap       = 5;
HM_GL_ChildOffset        = 10;
HM_GL_ChildPerCentOver   = null;
HM_GL_TopSecondsVisible  = .5;
HM_GL_StatusDisplayBuild = 0;
HM_GL_StatusDisplayLink  = 1;
HM_GL_UponDisplay        = null;
HM_GL_UponHide           = null;

//HM_GL_RightToLeft      = true;
HM_GL_CreateTopOnly      = HM_NS4 ? true : false;
HM_GL_ShowLinkCursor     = true;

if(HM_IsMenu) {	
	document.write("<SCR" + "IPT LANGUAGE='JavaScript1.2' SRC='/dyn/js/products/HM_Arrays2.js' TYPE='text/javascript'><\/SCR" + "IPT>");
	document.write("<SCR" + "IPT LANGUAGE='JavaScript1.2' SRC='/dyn/js/HM_Script"+ HM_BrowserString +".js' TYPE='text/javascript'><\/SCR" + "IPT>");
}

//end