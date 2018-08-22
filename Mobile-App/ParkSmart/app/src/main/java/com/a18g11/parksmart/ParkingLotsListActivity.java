package com.a18g11.parksmart;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.group18g11.parksmart.R;

import java.util.ArrayList;

public class ParkingLotsListActivity extends AppCompatActivity implements MyRecyclerViewAdapter.ItemClickListener {

    public static final int LOT_1 = 0;
    public static final int LOT_2 = 1;
    public static final int LOT_3 = 2;
    public static final int LOT_4 = 3;
    public static final int LOT_5 = 4;
    public static final int LOT_6 = 5;
    public static final int LOT_7 = 6;

    private char[][] parking_bays = new char[3][24];

    MyRecyclerViewAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_lots_list);

        int lot1_available = 0;
        int lot2_available = 0;
        int lot3_available = 0;
        int lot4_available = 0;
        int lot5_available = 0;
        int lot6_available = 0;
        int lot7_available = 0;

        parking_bays[0] = getIntent().getCharArrayExtra("row1");
        parking_bays[1] = getIntent().getCharArrayExtra("row2");
        parking_bays[2] = getIntent().getCharArrayExtra("row3");

        Log.i("Received data 1", String.valueOf(parking_bays[0]));
        Log.i("Received data 2", String.valueOf(parking_bays[1]));
        Log.i("Received data 3", String.valueOf(parking_bays[2]));

        for (char parking : parking_bays[0]){
            if (parking == '0') lot1_available++;
        }

        for (char parking : parking_bays[1]){
            if (parking == '0') lot1_available++;
        }

        for (char parking : parking_bays[2]) {
            if (parking == '0') lot1_available++;
        }

        // data to populate the RecyclerView with
        ArrayList<String> parkingLots = new ArrayList<>();
        parkingLots.add("Flower Hall");
        parkingLots.add("Demo Parking");
        parkingLots.add("Chamber of Mines");
        parkingLots.add("Amic Deck");
        parkingLots.add("FNB");
        parkingLots.add("2nd Year East");
        parkingLots.add("2nd Year West");

        ArrayList<Integer> availableParking = new ArrayList<>();
        availableParking.add(lot1_available);
        availableParking.add(lot2_available);
        availableParking.add(lot3_available);
        availableParking.add(lot4_available);
        availableParking.add(lot5_available);
        availableParking.add(lot6_available);
        availableParking.add(lot7_available);

        // set up the RecyclerView
        RecyclerView recyclerView = findViewById(R.id.parking_lots);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        adapter = new MyRecyclerViewAdapter(this, parkingLots, availableParking);
        adapter.setClickListener(this);
        recyclerView.setAdapter(adapter);

        // vertical list dividers for RecyclerView
        DividerItemDecoration decoration = new DividerItemDecoration(getApplicationContext(), DividerItemDecoration.VERTICAL);
        recyclerView.addItemDecoration(decoration);
    }

    @Override
    public void onItemClick(View view, int position) {
        Context context = view.getContext();

        switch(position){
            case LOT_1:
                Intent intent = new Intent(context, ParkingLot1.class);
                intent.putExtra("row1", parking_bays[0]);
                intent.putExtra("row2", parking_bays[1]);
                intent.putExtra("row3", parking_bays[2]);
                context.startActivity(intent);
                break;
            case LOT_2:
                break;
            case LOT_3:
                break;
            case LOT_4:
                break;
            case LOT_5:
                break;
            case LOT_6:
                break;
            case LOT_7:
                break;
            default:
                break;
        }
    }
}
