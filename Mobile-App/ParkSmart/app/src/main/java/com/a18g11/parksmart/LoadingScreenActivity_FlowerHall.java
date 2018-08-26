package com.a18g11.parksmart;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;

import com.group18g11.parksmart.R;

public class LoadingScreenActivity_FlowerHall extends Activity {

    private char[][] parking_bays = new char[3][24];

    //Introduce an delay
    private final int WAIT_TIME = 1000;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.loading_screen);
        findViewById(R.id.mainSpinner1).setVisibility(View.VISIBLE);
        parking_bays[0] = getIntent().getCharArrayExtra("row1");
        parking_bays[1] = getIntent().getCharArrayExtra("row2");
        parking_bays[2] = getIntent().getCharArrayExtra("row3");

        new Handler().postDelayed(new Runnable(){
            @Override
            public void run() {
                /* Create an Intent that will start the ProfileData-Activity. */
                Intent intent = new Intent(LoadingScreenActivity_FlowerHall.this, ParkingLot1.class);
                intent.putExtra("row1", parking_bays[0]);
                intent.putExtra("row2", parking_bays[1]);
                intent.putExtra("row3", parking_bays[2]);
                LoadingScreenActivity_FlowerHall.this.startActivity(intent);
                LoadingScreenActivity_FlowerHall.this.finish();
            }
        }, WAIT_TIME);
    }
}
