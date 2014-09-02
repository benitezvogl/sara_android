package cc.openframeworks.androidUiExample;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.net.Uri;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import cc.openframeworks.OFAndroid;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

import org.puredata.android.io.AudioParameters;
import org.puredata.android.io.PdAudio;
import org.puredata.core.PdBase;
import org.puredata.core.utils.IoUtils;

public class OFActivity extends cc.openframeworks.OFActivity{
	// the variable you want to share must be public
	public float value=0;
	public float camSelected=0;
	public float showDetails=0;
	public float _r = 0;
	public float _p = 0;
	public float _y = 0;
	public float _cr = 0;
	public float _cg = 0;
	public float _cb = 0;
	public int pd = 0;
	public int videoFilter = 0;
	private static final int MIN_SAMPLE_RATE = 44100;
	public OFAndroid ofApp;
	String packageName;
	ArrayList<Integer> mSettings = new ArrayList<Integer>();
	SharedPreferences prefs;
	
	@Override
    public void onCreate(Bundle savedInstanceState)
    { 
        super.onCreate(savedInstanceState);
        packageName = getPackageName();
        ofApp = new OFAndroid(packageName,this);
        
        prefs = getSharedPreferences("myprefs",Context.MODE_PRIVATE);
        int camera  = prefs.getInt("camera", 0); 
        mSettings.add(camera);
        value = (float)prefs.getInt("camera", 0);
        camSelected = (int)prefs.getInt("camera", 0);
        
        pd = prefs.getInt("pd", 0); 
        mSettings.add(pd);
        
        int show_data  = prefs.getInt("show_data", 0); 
        mSettings.add(show_data);
        
        videoFilter = prefs.getInt("vid", 0);
        mSettings.add(videoFilter);

        try {
			initPd();
		} catch (IOException e) {
			//toast(e.toString());
			finish();
		}
    }
	File patchFile = null;
	int handle = -1;
	private void LoadPDFile() {
		PdAudio.stopAudio();
		PdBase.closePatch(handle);
		
		Resources res = getResources();
		patchFile = null;
		
		if(pd == 0){
			InputStream in = res.openRawResource(R.raw.patch1_pry);
			try {
				patchFile = IoUtils.extractResource(in, "patch1_pry.pd", getCacheDir());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if(pd == 1){
			InputStream in = res.openRawResource(R.raw.patch1_rgb);
			try {
				patchFile = IoUtils.extractResource(in, "patch1_rgb.pd", getCacheDir());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if(pd == 2){
			InputStream in = res.openRawResource(R.raw.pry_synth);
			try {
				patchFile = IoUtils.extractResource(in, "pry_synth.pd", getCacheDir());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if(pd == 3){
			InputStream in = res.openRawResource(R.raw.rgb_synth);
			try {
				patchFile = IoUtils.extractResource(in, "rgb_synth.pd", getCacheDir());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		if(pd == 4){
			InputStream in = res.openRawResource(R.raw.arpeggio_synth_yrp_color);
			try {
				patchFile = IoUtils.extractResource(in, "arpeggio_synth_yrp_color.pd", getCacheDir());
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		try {
			handle = PdBase.openPatch(patchFile);
		} catch (IOException e) {
			e.printStackTrace();
		}
		PdAudio.startAudio(this);
	}
	private void initPd() throws IOException {
		AudioParameters.init(this);
		int srate = Math.max(MIN_SAMPLE_RATE, AudioParameters.suggestSampleRate());
		PdAudio.initAudio(srate, 0, 2, 1, true);
		LoadPDFile();	
	}
	@Override
	public void onDetachedFromWindow() {
		PdAudio.stopAudio();
		super.onDetachedFromWindow();
	}
	private void cleanup() {
		// make sure to release all resources
		PdAudio.release();
		PdBase.release();
		ofApp.destroy();
	}
	@Override
	protected void onDestroy() {
		cleanup();
		System.exit(0);
		super.onDestroy();
	}
	@Override
	protected void onStop() {
		PdAudio.stopAudio();
		super.onStop();
		System.exit(0);
	}
	
    @Override
    protected void onPause() {
        super.onPause();
		PdAudio.stopAudio();
        //ofApp.pause();
        ofApp.destroy();
        System.exit(0);
    }

    @Override
    protected void onResume() {
        super.onResume();
		PdAudio.startAudio(this);
		//ofApp.resume();
        ofApp = new OFAndroid(packageName,this);
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
	if (OFAndroid.keyDown(keyCode, event)) {
	    return true;
	} else {
	    return super.onKeyDown(keyCode, event);
	}
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
	if (OFAndroid.keyUp(keyCode, event)) {
	    return true;
	} else {
	    return super.onKeyUp(keyCode, event);
	}
    }


	
    
	
	
    // Menus
    // http://developer.android.com/guide/topics/ui/menus.html
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	// you inflate the menu
    	MenuInflater inflater=getMenuInflater();
    	inflater.inflate(R.menu.main_layout, menu);
    	
    	// Create settings menu options from here, one by one or infalting an xml
        return super.onCreateOptionsMenu(menu);
    }
    
    @SuppressWarnings("deprecation")
	public void showCameraAlert() {
    	AlertDialog alertDialog = new AlertDialog.Builder(this).create();
    	alertDialog.setTitle("Switch Camera");
		alertDialog.setMessage("You must close and re-open app for changes to take effect.");
		alertDialog.setButton("OK", new DialogInterface.OnClickListener() {
			   public void onClick(DialogInterface dialog, int which) {
			      
			   }
			});
		// Set the Icon for the Dialog
		alertDialog.setIcon(R.drawable.icon);
		alertDialog.show();
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	// This passes the menu option string to OF
    	// you can add additional behavior from java modifying this method
    	// but keep the call to OFAndroid so OF is notified of menu events
    	/*if(OFAndroid.menuItemSelected(item.getItemId())){
    		return true;
    	}*/
    	
    	//prefs = getSharedPreferences("myprefs",Context.MODE_PRIVATE);
    	SharedPreferences.Editor editor = prefs.edit(); 
        boolean reloadPD = false;
        boolean changeVid = false;
		Log.i("MyActivity", "Something Selected " + item.getItemId());
		
    	// you check which button of menu is pressed
    	switch(item.getItemId()){
    	
    	case R.id.back_camera :    		
    		editor.putInt("camera", 0); 
    		mSettings.set(0, 0);
    		camSelected=0;
    		showCameraAlert();    		
			break;
			
    	case R.id.front_camera:
    		editor.putInt("camera", 1); 
    		mSettings.set(0, 1);
    		camSelected=1;
    		showCameraAlert();
			break;

    	case R.id.display_on :
    		editor.putInt("show_data", 1);
    		mSettings.set(2, 1);
    		break;
    	case R.id.display_off :
    		editor.putInt("show_data", 0);
    		mSettings.set(2, 0);
    		break;
    		
	    		
		case R.id.accel_patch :
			pd = 0;
			reloadPD = true;
			break;
			//return true;
		case R.id.color_patch :
			pd = 1;
			reloadPD = true;
			break;
			//return true;
		case R.id.accel_synth_patch :
			pd = 2;
			reloadPD = true;
			break;
			//return true;
		case R.id.color_synth_patch :
			pd = 3;
			reloadPD = true;
			break;
			//return true;
		/*case R.id.arpeggio_synth :
			pd = 4;
			reloadPD = true;
			break;
			//return true;*/
			

		case R.id.none :
			videoFilter = 0;
			changeVid = true;
			break;
		case R.id.black_white :
			videoFilter = 1;
			changeVid = true;
			break;
		case R.id.invert :
			videoFilter = 2;
			changeVid = true;
			break;
		case R.id.pixels :
			videoFilter = 3;
			changeVid = true;
			break;
			
		case R.id.about_app :
			Intent viewIntent = new Intent("android.intent.action.VIEW", Uri.parse("http://benitezvogl.com/projects/sara"));
			startActivity(viewIntent); 
			break;
    	}
    	
    	if(changeVid){
    		editor.putInt("vid", videoFilter); 
    		mSettings.set(3, videoFilter);
    	}
    	
    	if(reloadPD){
    		Log.i("MyActivity", "LOAD PD: " + pd);
    		editor.putInt("pd", pd); 
    		mSettings.set(1, pd);
    		LoadPDFile();
    	}

		editor.commit(); 
    	return super.onOptionsItemSelected(item);
    }
    

    @Override
    public boolean onPrepareOptionsMenu (Menu menu){
    	// This method is called every time the menu is opened
    	//  you can add or remove menu options from here
    	return  super.onPrepareOptionsMenu(menu);
    }
    public int returnValueVideo() {
    	return mSettings.get(3);
    }
    public int returnValueDisplay(){
    	return mSettings.get(2);
    }
    // the function which will be called into oF 
    public float returnValue(float r, float p, float y, float cr, float cg, float cb){
    	_r = r;
    	_p = p;
    	_y = y;
    	_cr = cr;
    	_cg = cg;
    	_cb = cb;
    	PdBase.sendFloat("roll", _r);
		PdBase.sendFloat("pitch", _p);
		PdBase.sendFloat("yaw", _y);
		PdBase.sendFloat("red", _cr);
		PdBase.sendFloat("blue", _cb);
		PdBase.sendFloat("green", _cg);
    	float floatValue = camSelected;
    	return floatValue;
    }
   
	
}



