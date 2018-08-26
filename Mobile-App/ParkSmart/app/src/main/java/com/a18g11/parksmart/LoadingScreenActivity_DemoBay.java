package com.a18g11.parksmart;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;

import com.group18g11.parksmart.R;

public class LoadingScreenActivity_DemoBay extends Activity {

    //Introduce an delay
    private final int WAIT_TIME = 1000;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.loading_screen);

        findViewById(R.id.mainSpinner1).setVisibility(View.VISIBLE);
        final String demo_bay = getIntent().getStringExtra("demo_bay");

        new Handler().postDelayed(new Runnable(){
            @Override
            public void run() {
                /* Create an Intent that will start the ProfileData-Activity. */
                Intent intent = new Intent(LoadingScreenActivity_DemoBay.this, ParkingLot2.class);
                intent.putExtra("demo_bay", demo_bay);
                LoadingScreenActivity_DemoBay.this.startActivity(intent);
                LoadingScreenActivity_DemoBay.this.finish();
            }
        }, WAIT_TIME);
    }
}
