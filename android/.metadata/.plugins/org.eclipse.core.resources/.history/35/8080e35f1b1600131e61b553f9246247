package org.avplayer.avplayer;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;

import org.avplayer.avplayer.video.AndroidVideoWindowImpl;

import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.DialogInterface;
import android.content.Intent;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;

public class AvplayerActivity extends Activity {
	
	// 打开媒体类型.
	static int MEDIA_TYPE_FILE  = 0;
	static int MEDIA_TYPE_BT    = 1;
	static int MEDIA_TYPE_HTTP  = 2;
	static int  MEDIA_TYPE_RTSP = 3;
	static int  MEDIA_TYPE_YK   = 4;

	// 渲染模式.
	static int  RENDER_ANDROID_OGL = 0;

	AndroidVideoWindowImpl m_window;
	Thread m_sThread;
	boolean m_res = false;
	static {
 		
 		//ffmpeg
 		loadOptionalLibrary("avutil-2.0");
 		loadOptionalLibrary("avcodec-2.0");
 		loadOptionalLibrary("avformat-2.0");
 		loadOptionalLibrary("swscale-2.0");
 		loadOptionalLibrary("swresample-2.0");
  
 		// Main library
 		System.loadLibrary("avplayer");
 	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_avplayer);
		
		Log.v("java","onCreate");
		GLSurfaceView view = (GLSurfaceView) findViewById(R.id.video_surface);
		view.setZOrderOnTop(false);
	
		m_window = new AndroidVideoWindowImpl(view,null);
		m_window.init();
		
		URLDecoder d = new URLDecoder();
		
		Intent intent = getIntent();
		
		String filename = intent.getDataString();
		if(filename == null)
		{
			//test rtsp
			filename = "file://rtsp://218.204.223.237:554/live/1/66251FC11353191F/e7ooqwcfbqjoo80j.sdp";
			//test http
			//filename = "file://http://forum.ea3w.com/coll_ea3w/attach/2008_10/12237832415.3gp";
		}
	
		if(filename != null){
			
			try {
				filename = d.decode(filename, "utf-8");
			} catch (UnsupportedEncodingException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			AvplayerNative.AvplayInit();
			AvplayerNative.AvplaySetWindowHandle(m_window);
			
			filename = filename.substring(7);
			if(filename.indexOf(".torrent") >=0){
				
				m_res = AvplayerNative.AvplayOpen(filename, MEDIA_TYPE_BT, RENDER_ANDROID_OGL);
			}
			else{
				String is_url = filename.substring(0, 7);
				if(is_url.equals("http://")){
				
					m_res = AvplayerNative.AvplayOpen(filename, MEDIA_TYPE_HTTP, RENDER_ANDROID_OGL);
				}
				else if(is_url.equals("rtsp://")){
					
					m_res = AvplayerNative.AvplayOpen(filename, MEDIA_TYPE_RTSP, RENDER_ANDROID_OGL);
				}
				else{
					
					m_res = AvplayerNative.AvplayOpen(filename, MEDIA_TYPE_FILE, RENDER_ANDROID_OGL);
				}
				
			}
			
		
			if(m_res){
				
				m_sThread = new Thread() {
					public void run() {
						AvplayerNative.AvpalyPlay(0,0);
						AvplayerNative.AvplayWaitForCompletion();
						AvplayerNative.AvplayClose();
					};
				};
		
		
				/* start */
				m_sThread.start();
			}
			else{
				dialog("Vidoe Format Error");
			}
		}
	}
	
	@Override
	protected void onStop(){
		super.onStop();
		AvplayerNative.AvplayPause();
		Log.v("java","onStop");

	}
	
	@Override
	protected void onRestart(){
		super.onRestart();
		AvplayerNative.AvplayResume();
		Log.v("java","onRestart");
	}
	
	
	@Override
	protected void onDestroy(){
		super.onDestroy();
		Log.v("java","onDestroy");
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_avplayer, menu);
		return true;
	}
	
	 protected void dialog(String msg) { 
	        AlertDialog.Builder builder = new Builder(this); 
	        builder.setMessage(msg); 
	        builder.setTitle("提示"); 
	        builder.setPositiveButton("确认", 
	                new android.content.DialogInterface.OnClickListener() { 
	                    @Override 
	                    public void onClick(DialogInterface dialog, int which) { 
	                        dialog.dismiss(); 
	                        AvplayerNative.AvplayClose();
	                        AvplayerActivity.this.finish(); 
	                    }
	                }); 
	        builder.setNegativeButton("取消", 
	                new android.content.DialogInterface.OnClickListener() { 
	                    @Override 
	                    public void onClick(DialogInterface dialog, int which) { 
	                        dialog.dismiss(); 
	                    } 
	                }); 
	        builder.create().show(); 
	    } 
	    @Override 
	    public boolean onKeyDown(int keyCode, KeyEvent event) { 
	        if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) { 
	            dialog("确定要退出吗?"); 
	            return false; 
	        } 
	        return false; 
	    }
	
	private static void loadOptionalLibrary(String s) {
		try {
			System.loadLibrary(s);
		} catch (Throwable e) {
			Log.w("Unable to load optional library lib", s);
		}
	}
	
	private static String decodeUnicode(String theString) {  
	    char aChar;  
	    int len = theString.length();  
	    StringBuffer outBuffer = new StringBuffer(len);  
	    for (int x = 0; x < len;) {  
	        aChar = theString.charAt(x++);  
	        if (aChar == '\\') {  
	            aChar = theString.charAt(x++);  
	            if (aChar == 'u') {  
	                // Read the xxxx  
	                int value = 0;  
	                for (int i = 0; i < 4; i++) {  
	                    aChar = theString.charAt(x++);  
	                    switch (aChar) {  
	                    case '0':  
	                    case '1':  
	                    case '2':  
	                    case '3':  
	                    case '4':  
	                    case '5':  
	                    case '6':  
	                    case '7':  
	                    case '8':  
	                    case '9':  
	                        value = (value << 4) + aChar - '0';  
	                        break;  
	                    case 'a':  
	                    case 'b':  
	                    case 'c':  
	                    case 'd':  
	                    case 'e':  
	                    case 'f':  
	                        value = (value << 4) + 10 + aChar - 'a';  
	                        break;  
	                    case 'A':  
	                    case 'B':  
	                    case 'C':  
	                    case 'D':  
	                    case 'E':  
	                    case 'F':  
	                        value = (value << 4) + 10 + aChar - 'A';  
	                        break;  
	                    default:  
	                        throw new IllegalArgumentException(  
	                                "Malformed   \\uxxxx   encoding.");  
	                    }  
	  
	                }  
	                outBuffer.append((char) value);  
	            } else {  
	                if (aChar == 't')  
	                    aChar = '\t';  
	                else if (aChar == 'r')  
	                    aChar = '\r';  
	                else if (aChar == 'n')  
	                    aChar = '\n';  
	                else if (aChar == 'f')  
	                    aChar = '\f';  
	                outBuffer.append(aChar);  
	            }  
	        } else  
	            outBuffer.append(aChar);  
	    }  
	    return outBuffer.toString();  
	} 

}
