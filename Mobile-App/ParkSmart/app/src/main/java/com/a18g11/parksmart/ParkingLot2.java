package com.a18g11.parksmart;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
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

public class ParkingLot2 extends AppCompatActivity {

    private Context mContext;
    private Activity mActivity;
    private RelativeLayout mCLayout;

    GridView gridView;
    private String[] parkingNumbers = {"3", "3", "3", "3", "3", "3", "4", "4", "4"};
    private ArrayList<String> parkingOccupancy = new ArrayList<>();
    static final int update_time = 2000;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_parking_lot2);

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

        final String url = "https://api.thingspeak.com/channels/563558/fields/2&3&4.json?api_key=SL1GOUY5H6OB9HWZ&results=1&timezone=Africa%2FJohannesburg";

        // Get the application context
        mContext = getApplicationContext();
        mActivity = ParkingLot2.this;
        mCLayout = findViewById(R.id.relative_layout_parkings);

        String demo_bay = getIntent().getStringExtra("demo_bay");
        parkingNumbers[4] = demo_bay;

        for (String parking : parkingNumbers) {
            parkingOccupancy.add(parking);
        }

        gridView = findViewById(R.id.gridview_parkings);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, android.R.layout.simple_list_item_1, parkingNumbers){
                @NonNull
                @Override
                public View getView(int position, View convertView, @NonNull ViewGroup parent){
                // Cast the grid view current item as a text view
                TextView tv_cell = (TextView) super.getView(position,convertView,parent);
                tv_cell.setText("");

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
                tv_cell.getLayoutParams().height = height/3;
                tv_cell.getLayoutParams().width = width/3;

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
              //Log.i("Timer", "Updated");
              JsonObjectRequest jsonObjectRequest = new JsonObjectRequest
                  (Request.Method.GET, url, null, new Response.Listener<JSONObject>() {

                      @Override
                      public void onResponse(JSONObject response) {
                          //Log.i("API Resp ", response.toString());
                          try {
                              JSONArray feeds = response.getJSONArray("feeds");
                              JSONObject latest_feed = feeds.getJSONObject(0);
                              String demo_bay = latest_feed.get("field2").toString();

                              //Log.i("Feed ", feeds.toString());
                              //Log.i("Latest Feed ", latest_feed.get("field2").toString());
                              //Log.i("Demo Bay ", String.valueOf(demo_bay));

                              parkingOccupancy.clear();
                              parkingNumbers[4] = demo_bay;

                              for (String parking : parkingNumbers) {
                                  parkingOccupancy.add(parking);
                              }

                              ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext, android.R.layout.simple_list_item_1, parkingNumbers){
                                  @NonNull
                                  @Override
                                  public View getView(int position, View convertView, @NonNull ViewGroup parent){
                                      // Cast the grid view current item as a text view
                                      TextView tv_cell = (TextView) super.getView(position,convertView,parent);
                                      tv_cell.setText("");

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
                                      tv_cell.getLayoutParams().height = height/3;
                                      tv_cell.getLayoutParams().width = width/3;

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
