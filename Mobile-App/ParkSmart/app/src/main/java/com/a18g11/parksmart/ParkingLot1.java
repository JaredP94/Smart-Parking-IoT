package com.a18g11.parksmart;

import android.app.Activity;
import android.content.Context;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.GridView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.group18g11.parksmart.R;

public class ParkingLot1 extends AppCompatActivity {

    private Context mContext;
    private Activity mActivity;
    private RelativeLayout mCLayout;

    GridView gridView;
    private String[] parkingNumbers = new String[28*14];
    private Integer[] invalidParkings = new Integer[10];

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_lot1);

        // Get the application context
        mContext = getApplicationContext();
        mActivity = ParkingLot1.this;
        mCLayout = findViewById(R.id.relative_layout_parkings);

        Integer rowCounter = 0;

        for(int index = 0; index < parkingNumbers.length; index++){
            if(index % 28 == 0) rowCounter = 0;
            if(index < 28 || index >= 84 && index < 112 || index >= 168 && index < 196 || index >= 252 && index < 280 || index >= 336 && index < 364){
                parkingNumbers[index] = " ";
            }
            else if((index % 28) % 15 == 0 && rowCounter!=1) {
                parkingNumbers[index] = "1";
                rowCounter++;
            }
            else if((index % 28) % 15 == 0 && rowCounter==1) {
                parkingNumbers[index] = " ";
                rowCounter++;
            }
            else{
                parkingNumbers[index] = "1";
            }
        }

        gridView = (GridView) findViewById(R.id.gridview_parkings);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, android.R.layout.simple_list_item_1, parkingNumbers){
                @Override
                public View getView(int position, View convertView, ViewGroup parent){
                // Cast the grid view current item as a text view
                TextView tv_cell = (TextView) super.getView(position,convertView,parent);

                // Set the item background drawable
                if(parkingNumbers[position] == "1") {
                    tv_cell.setBackground(
                            ContextCompat.getDrawable(
                                    mContext, R.drawable.gridview_item_background
                            )
                    );
                }

                // Put item item text in cell center
                tv_cell.setGravity(Gravity.CENTER);

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
                tv_cell.getLayoutParams().height = 71;
                tv_cell.getLayoutParams().width = 65;

                // Return the modified item
                return tv_cell;
            };
        };
        gridView.setAdapter(adapter);
    }
}
