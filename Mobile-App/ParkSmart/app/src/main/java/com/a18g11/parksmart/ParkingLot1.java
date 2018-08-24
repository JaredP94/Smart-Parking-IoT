package com.a18g11.parksmart;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.GridView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.android.volley.Cache;
import com.android.volley.Network;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.BasicNetwork;
import com.android.volley.toolbox.DiskBasedCache;
import com.android.volley.toolbox.HurlStack;
import com.android.volley.toolbox.JsonObjectRequest;
import com.group18g11.parksmart.R;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

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
    static final int update_time = 5000;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_lot1);

        DisplayMetrics dm = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(dm);
        int resId = getResources().getIdentifier("status_bar_height", "dimen", "android");
        int result = 0;
        if (resId > 0) {
            result = getResources().getDimensionPixelSize(resId);
        }
        final int width = dm.widthPixels;
        final int height = dm.heightPixels - result;

        final RequestQueue mRequestQueue;

        // Instantiate the cache
        Cache cache = new DiskBasedCache(getCacheDir(), 1024 * 1024); // 1MB cap

        // Set up the network to use HttpURLConnection as the HTTP client.
        Network network = new BasicNetwork(new HurlStack());

        // Instantiate the RequestQueue with the cache and network.
        mRequestQueue = new RequestQueue(cache, network);

        // Start the queue
        mRequestQueue.start();

        final String url = "https://api.thingspeak.com/channels/542645/fields/2&3&4.json?api_key=GX2QFYAFHEZ86AIO&results=1&timezone=Africa%2FJohannesburg";

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
                @NonNull
                @Override
                public View getView(int position, View convertView, @NonNull ViewGroup parent){
                // Cast the grid view current item as a text view
                TextView tv_cell = (TextView) super.getView(position,convertView,parent);

                if(position == 25){
                    tv_cell.setTextColor(Color.BLACK);
                    tv_cell.setTextSize(20);
                    tv_cell.setText(R.string.entrance);
                }

                // Set the item background drawable
                    switch (parkingNumbers[position]) {
                        case "3":
                            tv_cell.setBackground(
                                    ContextCompat.getDrawable(
                                            mContext, R.drawable.gridview_item_background_uncovered_parkings
                                    )
                            );
                            break;
                        case "1":
                            tv_cell.setBackground(
                                    ContextCompat.getDrawable(
                                            mContext, R.drawable.gridview_item_background_occupied_parking
                                    )
                            );
                            break;
                        case "0":
                            tv_cell.setBackground(
                                    ContextCompat.getDrawable(
                                            mContext, R.drawable.gridview_item_background_unoccupied_parking
                                    )
                            );
                            break;
                        case "2":
                            tv_cell.setBackground(
                                    ContextCompat.getDrawable(
                                            mContext, R.drawable.gridview_item_background_node_error
                                    )
                            );
                            break;
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
                    tv_cell.getLayoutParams().height = height/14;
                    tv_cell.getLayoutParams().width = width/28;
                }

                // Return the modified item
                return tv_cell;
            }
        };

        gridView.setAdapter(adapter);

        Timer t = new Timer();

        //Set the schedule function and rate
        t.scheduleAtFixedRate(new TimerTask() {
          @Override
          public void run() {
              Log.i("Timer", "Updated");
              JsonObjectRequest jsonObjectRequest = new JsonObjectRequest
                  (Request.Method.GET, url, null, new Response.Listener<JSONObject>() {

                      @Override
                      public void onResponse(JSONObject response) {
                          Log.i("API Resp ", response.toString());
                          //data[0] = response.getJSONObject("channel");
                          try {
                              JSONArray feeds = response.getJSONArray("feeds");
                              JSONObject latest_feed = feeds.getJSONObject(0);
                              parking_bays[0] = latest_feed.get("field2").toString().toCharArray();
                              parking_bays[1] = latest_feed.get("field3").toString().toCharArray();
                              parking_bays[2] = latest_feed.get("field4").toString().toCharArray();

                              Log.i("Feed ", feeds.toString());
                              Log.i("Latest Feed ", latest_feed.get("field2").toString());
                              Log.i("Parking Bays 1 ", String.valueOf(parking_bays[0]));
                              Log.i("Parking Bays 2 ", String.valueOf(parking_bays[1]));
                              Log.i("Parking Bays 3 ", String.valueOf(parking_bays[2]));

                              int track_index = 0;
                              parkingOccupancy.clear();

                              for (int i = 0; i < 3; i++){
                                  for (int j = 0; j < 24; j++){
                                      parkingOccupancy.add(parking_bays[i][j]);
                                  }
                              }

                              for(int index : trackedParkings){
                                  parkingNumbers[index] = parkingOccupancy.get(track_index).toString();
                                  track_index++;
                              }

                              ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, android.R.layout.simple_list_item_1, parkingNumbers){
                                  @NonNull
                                  @Override
                                  public View getView(int position, View convertView, @NonNull ViewGroup parent){
                                      // Cast the grid view current item as a text view
                                      TextView tv_cell = (TextView) super.getView(position,convertView,parent);

                                      if(position == 25){
                                          tv_cell.setTextColor(Color.BLACK);
                                          tv_cell.setTextSize(20);
                                          tv_cell.setText(R.string.entrance);
                                      }

                                      // Set the item background drawable
                                      switch (parkingNumbers[position]) {
                                          case "3":
                                              tv_cell.setBackground(
                                                      ContextCompat.getDrawable(
                                                              mContext, R.drawable.gridview_item_background_uncovered_parkings
                                                      )
                                              );
                                              break;
                                          case "1":
                                              tv_cell.setBackground(
                                                      ContextCompat.getDrawable(
                                                              mContext, R.drawable.gridview_item_background_occupied_parking
                                                      )
                                              );
                                              break;
                                          case "0":
                                              tv_cell.setBackground(
                                                      ContextCompat.getDrawable(
                                                              mContext, R.drawable.gridview_item_background_unoccupied_parking
                                                      )
                                              );
                                              break;
                                          case "2":
                                              tv_cell.setBackground(
                                                      ContextCompat.getDrawable(
                                                              mContext, R.drawable.gridview_item_background_node_error
                                                      )
                                              );
                                              break;
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
                                          tv_cell.getLayoutParams().height = height/14;
                                          tv_cell.getLayoutParams().width = width/28;
                                      }

                                      // Return the modified item
                                      return tv_cell;
                                  }
                              };

                          adapter.notifyDataSetChanged();
                          gridView.setAdapter(adapter);

                          } catch (JSONException e) {
                              e.printStackTrace();
                          }
                      }
                  }, new Response.ErrorListener() {

                      @Override
                      public void onErrorResponse(VolleyError error) {
                          // TODO: Handle error

                      }
                  });

              // Add the request to the RequestQueue.
              mRequestQueue.add(jsonObjectRequest);
          }
      },
    //Set how long before to start calling the TimerTask (in milliseconds)
    update_time,
    //Set the amount of time between each execution (in milliseconds)
    update_time);
    }
}
