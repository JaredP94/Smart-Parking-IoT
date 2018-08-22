package com.a18g11.parksmart;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.GridView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.group18g11.parksmart.R;

import java.util.ArrayList;

public class ParkingLot1 extends AppCompatActivity {

    private Context mContext;
    private Activity mActivity;
    private RelativeLayout mCLayout;

    GridView gridView;
    private String[] parkingNumbers = new String[28*14];
    private char[][] parking_bays = new char[3][24];
    private Integer[] trackedParkings = {
            128, 129, 156, 157, 130, 131, 158, 159, 132, 133, 160, 161, 134, 135, 162, 163, 136, 137, 164, 165, 138, 139, 166, 167,
            212, 213, 240, 241, 214, 215, 242, 243, 216, 217, 244, 245, 218, 219, 246, 247, 220, 221, 248, 249, 222, 223, 250, 251,
            296, 297, 324, 325, 298, 299, 326, 327, 300, 301, 328, 329, 302, 303, 330, 331, 304, 305, 332, 333, 306, 307, 334, 335
        };
    private ArrayList<Character> parkingOccupancy = new ArrayList<>();

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_lot1);

        // Get the application context
        mContext = getApplicationContext();
        mActivity = ParkingLot1.this;
        mCLayout = findViewById(R.id.relative_layout_parkings);

        parking_bays[0] = getIntent().getCharArrayExtra("row1");
        parking_bays[1] = getIntent().getCharArrayExtra("row2");
        parking_bays[2] = getIntent().getCharArrayExtra("row3");

        int rowCounter = 0;
        int track_index = 0;

        for(int index = 0; index < parkingNumbers.length; index++){
            if(index % 28 == 0) rowCounter = 0;
            if(index < 28 || index >= 84 && index < 112 || index >= 168 && index < 196 || index >= 252 && index < 280 || index >= 336 && index < 364){
                parkingNumbers[index] = " ";
            }
            else if((index % 28) % 15 == 0 && rowCounter!=1) {
                parkingNumbers[index] = "3";
                rowCounter++;
            }
            else if((index % 28) % 15 == 0 && rowCounter==1) {
                parkingNumbers[index] = " ";
                rowCounter++;
            }
            else{
                parkingNumbers[index] = "3";
            }
        }

        for (int i = 0; i < 3; i++){
            for (int j = 0; j < 24; j++){
                parkingOccupancy.add(parking_bays[i][j]);
            }
        }


        for(int index : trackedParkings){
            parkingNumbers[index] = parkingOccupancy.get(track_index).toString();
            track_index++;
        }

        gridView = (GridView) findViewById(R.id.gridview_parkings);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, android.R.layout.simple_list_item_1, parkingNumbers){
                @Override
                public View getView(int position, View convertView, ViewGroup parent){
                // Cast the grid view current item as a text view
                TextView tv_cell = (TextView) super.getView(position,convertView,parent);

                if(position == 25){
                    tv_cell.setTextColor(Color.BLACK);
                    tv_cell.setTextSize(20);
                    tv_cell.setText("Entrance");
                }

                // Set the item background drawable
                if(parkingNumbers[position].equals("3")) {
                    tv_cell.setBackground(
                            ContextCompat.getDrawable(
                                    mContext, R.drawable.gridview_item_background_uncovered_parkings
                            )
                    );
                }
                else if(parkingNumbers[position].equals("1")) {
                    tv_cell.setBackground(
                            ContextCompat.getDrawable(
                                    mContext, R.drawable.gridview_item_background_occupied_parking
                            )
                    );
                }

                else if(parkingNumbers[position].equals("0")) {
                    tv_cell.setBackground(
                            ContextCompat.getDrawable(
                                    mContext, R.drawable.gridview_item_background_unoccupied_parking
                            )
                    );
                }

                else if(parkingNumbers[position].equals("2")) {
                    tv_cell.setBackground(
                            ContextCompat.getDrawable(
                                    mContext, R.drawable.gridview_item_background_node_error
                            )
                    );
                }



                // Put item item text in cell center
                if(position == 25) {
                    tv_cell.setGravity(Gravity.TOP);
                }
                else {
                    tv_cell.setGravity(Gravity.CENTER);
                }

                /*
                    void setHeight (int pixels)
                        Makes the TextView exactly this many pixels tall. You could do the same
                        thing by specifying this number in the LayoutParams. Note that
                        setting this value overrides any other
                        (minimum / maximum) number of lines or height setting.

                        Parameters
                            pixels : int
                */

                // Set the grid view item/cell/row height
                //tv_cell.setHeight(300); // In pixels

                // Another way to change grid view row height
                if(position == 25) {
                    tv_cell.getLayoutParams().height = 100;
                    tv_cell.getLayoutParams().width = 350;
                }
                else{
                    tv_cell.getLayoutParams().height = 71;
                    tv_cell.getLayoutParams().width = 65;
                }

                // Return the modified item
                return tv_cell;
            }
        };
        gridView.setAdapter(adapter);
    }
}
