
package com.kviation.sample.orientation;

import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements Orientation.Listener {

  private Orientation mOrientation;
  private RCFocus mRCFocus;
  private cBluetooth bl = null;
  private ImageButton lightSwitch, accelerateButton, brakeButton, shiftUpButton, shiftDownButton;
  private boolean lightSwitchOn = false;
  private boolean isPlaying = false;
  private boolean acceleratePressed = false;
  private Drawable lightSwitchImage, gearBoxUImage, gearboxDImage;
  private Button nextButton, backButton, stopButton, volUpButton, volDownButton;
  private TextView playerText;
  private String accelerateCommand = "ACC";
  private String brakeCommand = "BRA";
  private String lightCommand = "LIG";
  private String angleCommand = "ANG";
  private String shiftCommand = "SHI";
  private String messageClose = "@";

  private String address = "00:21:13:00:F8:A2"; //Address of the BT
  //private String address = "5C:2E:59:0B:82:58"; //Address of the BT
  private long elapsedTime;


  Handler gearHandler = new Handler();
  Handler angleSendHandler = new Handler();

  Runnable runnable = new Runnable() {
    public void run() {
      gearHandler.postDelayed(this, 1000);
      if (elapsedTime > 1000 && !acceleratePressed){
        if (mRCFocus.getShift() != 1 && mRCFocus.getShift() != -1 && mRCFocus.getShift() != 0) {
          mRCFocus.setShift(1);
        }
      }
    }
  };
  Runnable angleSendRunnable = new Runnable() {
    @Override
    public void run() {
     bl.sendData(angleCommand + String.valueOf((int)mRCFocus.getRoll()) + messageClose/* + "\n"*/);
     angleSendHandler.postDelayed(this, 333);
    }
  };

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
    setContentView(R.layout.main);

    mOrientation = new Orientation(this);
    mRCFocus = (RCFocus) findViewById(R.id.rc_focus);
    playerText = (TextView) findViewById(R.id.radioText);
    nextButton = (Button) findViewById(R.id.nextButton);
    backButton = (Button) findViewById(R.id.backButton);
    lightSwitch = (ImageButton) findViewById(R.id.lightSwitch);
    stopButton = (Button) findViewById(R.id.stopButton);
    volUpButton = (Button) findViewById(R.id.volUpButton);
    volDownButton = (Button) findViewById(R.id.volDownButton);
    accelerateButton = (ImageButton) findViewById(R.id.accelerate);
    brakeButton = (ImageButton) findViewById(R.id.brake);
    shiftDownButton = (ImageButton) findViewById(R.id.shiftDown);
    shiftUpButton = (ImageButton) findViewById(R.id.shiftUp);
    elapsedTime = System.currentTimeMillis();


    ActionBar actionBar = getSupportActionBar();
    actionBar.setDisplayShowHomeEnabled(true);
    actionBar.setIcon(R.mipmap.ford_icon);

    bl = new cBluetooth(this, mHandler);
    bl.checkBTState();
    angleSendHandler.postDelayed(angleSendRunnable, 2000);
    gearHandler.postDelayed(runnable, 1000);

    lightSwitch.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        if (lightSwitchOn) {
          lightSwitchImage = getResources().getDrawable(R.drawable.lampa_kapcsolo_ki);
          lightSwitchOn = false;
          bl.sendData(lightCommand + "-1" + messageClose + "\r");
        } else {
          lightSwitchImage = getResources().getDrawable(R.drawable.lampa_kapcsolo_be);
          lightSwitchOn = true;
          bl.sendData(lightCommand + "+1" + messageClose + "\r");
        }
        lightSwitch.setImageDrawable(lightSwitchImage);
      }
    });


    brakeButton.setOnTouchListener(new View.OnTouchListener() {
      @Override
      public boolean onTouch(View view, MotionEvent motionEvent) {
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN){
          bl.sendData(brakeCommand + "+1" + messageClose + "\r");
          mRCFocus.setShift(0);
        }else if (motionEvent.getAction() == MotionEvent.ACTION_UP){
          bl.sendData(brakeCommand + "-1" + messageClose + "\r");
        }
        return false;
      }
    });

    accelerateButton.setOnTouchListener(new View.OnTouchListener() {
      @Override
      public boolean onTouch(View view, MotionEvent motionEvent) {
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN){
          acceleratePressed = true;
          bl.sendData(accelerateCommand + "+1" + messageClose + "\r");
          elapsedTime = System.currentTimeMillis() - elapsedTime;
        }else if (motionEvent.getAction() == MotionEvent.ACTION_UP){
          acceleratePressed = false;
          bl.sendData(accelerateCommand + "-1" + messageClose + "\r");
          elapsedTime = System.currentTimeMillis() - elapsedTime;
        }
        return false;
      }
    });


    nextButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        /*BT send next to playes*/
        playerText.setText("Next");
      }
    });

    backButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        /*BT send next to playes*/
        playerText.setText("Prev");
      }
    });

    stopButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        /*BT send next to playes*/
        if (isPlaying) {
          isPlaying = false;
          playerText.setText("STOP");
        } else {
          isPlaying = true;
          playerText.setText("Play");
        }
      }
    });

    volDownButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        /*BT send next to playes*/
        playerText.setText("Vol -");
      }
    });

    volUpButton.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View view) {
        /*BT send next to playes*/
        playerText.setText("Vol +");
      }
    });

    shiftUpButton.setOnTouchListener(new View.OnTouchListener() {
      @Override
      public boolean onTouch(View view, MotionEvent motionEvent) {
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN){
          gearBoxUImage = getResources().getDrawable(R.drawable.valto_egesz);
          gearboxDImage = getResources().getDrawable(R.drawable.valto_ures);

        }else if (motionEvent.getAction() == MotionEvent.ACTION_UP){
          if (acceleratePressed || mRCFocus.getShift() < 1){
            mRCFocus.setShift(mRCFocus.getShift() + 1);
          }
          bl.sendData(shiftCommand + String.valueOf(mRCFocus.getShift()) + messageClose + "\r");
          gearBoxUImage = getResources().getDrawable(R.drawable.valto_felul);
          gearboxDImage = getResources().getDrawable(R.drawable.valto_alul);
        }
        elapsedTime = System.currentTimeMillis() - elapsedTime;
        shiftUpButton.setImageDrawable(gearBoxUImage);
        shiftDownButton.setImageDrawable(gearboxDImage);
        return false;
      }
    });


    shiftDownButton.setOnTouchListener(new View.OnTouchListener() {
      @Override
      public boolean onTouch(View view, MotionEvent motionEvent) {
        if (motionEvent.getAction() == MotionEvent.ACTION_DOWN){
          gearBoxUImage = getResources().getDrawable(R.drawable.valto_ures);
          gearboxDImage = getResources().getDrawable(R.drawable.valto_egesz);

        }else if (motionEvent.getAction() == MotionEvent.ACTION_UP){
          mRCFocus.setShift(mRCFocus.getShift() - 1);
          bl.sendData(shiftCommand + String.valueOf(mRCFocus.getShift()) + messageClose + "\r");
          gearBoxUImage = getResources().getDrawable(R.drawable.valto_felul);
          gearboxDImage = getResources().getDrawable(R.drawable.valto_alul);
        }
        elapsedTime = System.currentTimeMillis() - elapsedTime;
        shiftUpButton.setImageDrawable(gearBoxUImage);
        shiftDownButton.setImageDrawable(gearboxDImage);
        return false;
      }
    });
  }


  @Override
  protected void onStart() {
    super.onStart();
    mOrientation.startListening(this);
    bl.BT_Connect(address);
  }

  @Override
  protected void onResume() {
    super.onResume();
    //try {
      //bl.BT_Connect(address);
    //}catch (Exception e){
    //  bl.BT_Connect(address);
    //}
  }

  @Override
  protected void onPause() {
    super.onPause();
    //bl.BT_onPause();
  }

  @Override
  protected void onStop() {
    super.onStop();
    mOrientation.stopListening();
    bl.BT_onPause();
  }

  @Override
  public void onOrientationChanged(float pitch, float roll) {
    mRCFocus.setAttitude(pitch, roll);
  }

  @Override
  protected void onDestroy() {
    super.onDestroy();
    angleSendHandler.removeCallbacks(angleSendRunnable);
  }

  private final Handler mHandler = new Handler() {
    public void handleMessage(android.os.Message msg) {
      switch (msg.what) {
        case cBluetooth.BL_NOT_AVAILABLE:
          Log.d(cBluetooth.TAG, "Bluetooth is not available. Exit");
          Toast.makeText(getBaseContext(), "Bluetooth is not available", Toast.LENGTH_SHORT).show();
          finish();
          break;
        case cBluetooth.BL_INCORRECT_ADDRESS:
          Log.d(cBluetooth.TAG, "Incorrect MAC address");
          Toast.makeText(getBaseContext(), "Incorrect Bluetooth address", Toast.LENGTH_SHORT).show();
          break;
        case cBluetooth.BL_REQUEST_ENABLE:
          Log.d(cBluetooth.TAG, "Request Bluetooth Enable");
          BluetoothAdapter.getDefaultAdapter();
          Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
          startActivityForResult(enableBtIntent, 1);
          break;
        case cBluetooth.BL_SOCKET_FAILED:
          Toast.makeText(getBaseContext(), "Socket failed", Toast.LENGTH_SHORT).show();
          bl.BT_Connect(address);
          //finish();
          break;
      }
    }
  };
}