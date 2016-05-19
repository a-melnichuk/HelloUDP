package com.example.hellojni;

import android.app.Activity;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.os.Bundle;


public class HelloUdp extends Activity
{

    TextView response;
    TextView request;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);


        setContentView(R.layout.main);
        request = (TextView) findViewById(R.id.request);
        response = (TextView) findViewById(R.id.response);

        final EditText ip = (EditText) findViewById(R.id.ip);
        final EditText port = (EditText) findViewById(R.id.port);

        findViewById(R.id.connect).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String portStr = port.getText().toString();
                if(portStr.isEmpty()) return;

                int info = connectionInfoToJni(ip.getText().toString(), Integer.valueOf(portStr));

            }
        });
    }

    public native int connectionInfoToJni(String id, int port);


    public void appendToResponse(String text) {
        appendToTextView(response, text);
    }

    public void appendToRequest(String text) {
       appendToTextView(request, text);
    }

    private void appendToTextView(TextView t, String text) {
        if(t != null) {
            String newText = t.getText() + text;
            t.setText(newText);
        }
    }

    static {
        System.loadLibrary("hello-udp");
    }
}
