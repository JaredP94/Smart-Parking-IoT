package com.a18g11.parksmart;

import android.content.Intent;
import android.os.Handler;
import android.preference.PreferenceActivity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.android.volley.Cache;
import com.android.volley.Network;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.BasicNetwork;
import com.android.volley.toolbox.DiskBasedCache;
import com.android.volley.toolbox.HurlStack;
import com.android.volley.toolbox.JsonArrayRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;
import com.group18g11.parksmart.R;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.android.volley.Request;

public class SplashScreenActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.splash_screen);

        RequestQueue mRequestQueue;

        // Instantiate the cache
        Cache cache = new DiskBasedCache(getCacheDir(), 1024 * 1024); // 1MB cap

        // Set up the network to use HttpURLConnection as the HTTP client.
        Network network = new BasicNetwork(new HurlStack());

        // Instantiate the RequestQueue with the cache and network.
        mRequestQueue = new RequestQueue(cache, network);

        // Start the queue
        mRequestQueue.start();

        String url_lot1 = "https://api.thingspeak.com/channels/542645/fields/2&3&4.json?api_key=GX2QFYAFHEZ86AIO&results=1&timezone=Africa%2FJohannesburg";
        String url_lot2 = "https://api.thingspeak.com/channels/563558/fields/2&3&4.json?api_key=SL1GOUY5H6OB9HWZ&results=1&timezone=Africa%2FJohannesburg";

        final char[][] parking_bays = new char[3][24];
        final Intent lot_loader = new Intent(this, ParkingLotsListActivity.class);
        final Boolean[] lot1_dl = {false};
        final Boolean[] lot2_dl = {false};

        JsonObjectRequest flower_hall_parking = new JsonObjectRequest
                (Request.Method.GET, url_lot1, null, new Response.Listener<JSONObject>() {

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
                            Log.i("Parking Bays 2 ", String.valueOf(parking_bays[0]));
                            Log.i("Parking Bays 3 ", String.valueOf(parking_bays[0]));

                            lot_loader.putExtra("row1", parking_bays[0]);
                            lot_loader.putExtra("row2", parking_bays[1]);
                            lot_loader.putExtra("row3", parking_bays[2]);

                            lot1_dl[0] = true;

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

        JsonObjectRequest demo_bay_parking = new JsonObjectRequest
                (Request.Method.GET, url_lot2, null, new Response.Listener<JSONObject>() {

                    @Override
                    public void onResponse(JSONObject response) {
                        Log.i("API Resp ", response.toString());
                        //data[0] = response.getJSONObject("channel");
                        try {
                            JSONArray feeds = response.getJSONArray("feeds");
                            JSONObject latest_feed = feeds.getJSONObject(0);
                            String demo_bay = latest_feed.get("field2").toString();

                            Log.i("Feed ", feeds.toString());
                            Log.i("Latest Feed ", latest_feed.get("field2").toString());
                            Log.i("Demo Bay", String.valueOf(demo_bay));

                            lot_loader.putExtra("demo_bay", demo_bay);

                            lot2_dl[0] = true;

                            while (lot1_dl[0] != true || lot2_dl[0] != true) { }

                            startActivity(lot_loader);
                            finish();
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
        mRequestQueue.add(flower_hall_parking);
        mRequestQueue.add(demo_bay_parking);
    }

}
