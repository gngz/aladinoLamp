package com.example.vitor.aladinoslamp;

    import android.app.Activity;
    import android.graphics.Color;
    //import android.support.v7.app.AppCompatActivity;
    import android.os.Bundle;
    import android.view.View;
    import android.view.View.OnClickListener;
    import android.widget.Button;
    import android.widget.RelativeLayout;
    import android.widget.TextView;
    import android.widget.EditText;
    import net.margaritov.preference.colorpicker.ColorPickerDialog;

    import java.io.IOException;

public class MainActivity extends Activity {

    public final byte SETDATE = 4;
    private final byte SETTIME = 5;
    private final byte SETCOLOR = 3;
    private final byte SETMODE = 2;
    private final byte  ACK = 0; //echo
    private final byte setMode = 3;
    private final byte  requestTemp = 6;
    private final byte  sendTemp = 7;
    Button but;
    TextView textV;
    RelativeLayout rlayout;
    TextView response;
    EditText editTextAddress, editTextPort;
    Button buttonConnect, buttonClear;

    ColorPickerDialog colorPickerDialog;
    int color = Color.parseColor("#33b5e5");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        editTextAddress = (EditText) findViewById(R.id.addressEditText);
        editTextPort = (EditText) findViewById(R.id.portEditText);
        buttonConnect = (Button) findViewById(R.id.connectButton);
        buttonClear = (Button) findViewById(R.id.clearButton);
        response = (TextView) findViewById(R.id.responseTextView);
        //tv = (TextView) findViewById(R.id.textView);
        rlayout = (RelativeLayout) findViewById(R.id.layout);
        but = (Button) findViewById(R.id.selCor);
        but.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                colorPickerDialog = new ColorPickerDialog(MainActivity.this,color );
                colorPickerDialog.setAlphaSliderVisible(true);
                colorPickerDialog.setHexValueEnabled(true);
                colorPickerDialog.setTitle("Pick a Color: ");
                colorPickerDialog.setOnColorChangedListener(new ColorPickerDialog.OnColorChangedListener() {
                    @Override
                    public void onColorChanged(int i) {
                        color = i;

                    }
                });

                colorPickerDialog.show();

            }
        });

        buttonConnect.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                Client myClient = new Client(editTextAddress.getText()
                        .toString(), Integer.parseInt(editTextPort
                        .getText().toString()), response);
                myClient.execute();
                try {
                    myClient.sendData(ACK);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        buttonClear.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                response.setText("");
            }
        });
    }
}
