package com.a18g11.parksmart;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DividerItemDecoration;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
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

    MyRecyclerViewAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_lots_list);

        // data to populate the RecyclerView with
        ArrayList<String> parkingLots = new ArrayList<>();
        parkingLots.add("Parking Lot 1 \t Value");
        parkingLots.add("Parking Lot 2 \t Value");
        parkingLots.add("Parking Lot 3 \t Value");
        parkingLots.add("Parking Lot 4 \t Value");
        parkingLots.add("Parking Lot 5 \t Value");

        // set up the RecyclerView
        RecyclerView recyclerView = findViewById(R.id.parking_lots);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        adapter = new MyRecyclerViewAdapter(this, parkingLots);
        adapter.setClickListener(this);
        recyclerView.setAdapter(adapter);

        // vertical list dividers for RecyclerView
        DividerItemDecoration decoration = new DividerItemDecoration(getApplicationContext(), DividerItemDecoration.VERTICAL);
        recyclerView.addItemDecoration(decoration);
    }

    @Override
    public void onItemClick(View view, int position) {
        Toast.makeText(this, "You clicked " + adapter.getItem(position) + " on row number " + position, Toast.LENGTH_SHORT).show();
        Context context = view.getContext();

        switch(position){
            case LOT_1:
                Intent intent = new Intent(context, ParkingLot1.class);
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
            default:
                break;
        }
    }
}
