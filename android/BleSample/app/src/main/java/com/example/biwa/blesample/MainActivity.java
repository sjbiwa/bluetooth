package com.example.biwa.blesample;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.UUID;

//import android.support.v7.app.AppCompatActivity;

public class MainActivity extends Activity implements BluetoothAdapter.LeScanCallback, AdapterView.OnItemClickListener {

    // 対象のサービスUUID.
    private static final String SERVICE_UUID = "07231001-B201-4924-8A0A-1F5EC5AA8FB9";
    // キャラクタリスティックUUID.
    private static final String CHARACTERISTIC_UUID = "07231002-B201-4924-8A0A-1F5EC5AA8FB9";

    class MyGatt extends BluetoothGattCallback {

        public MyGatt() {
            super();
        }

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);

            if ( newState == BluetoothProfile.STATE_CONNECTED ) {
                gatt.discoverServices();
            } else if ( newState == BluetoothProfile.STATE_DISCONNECTED ) {
                gatt.close();
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);

            // Serviceが見つかったら実行.
            if ( status == BluetoothGatt.GATT_SUCCESS ) {
                // UUIDが同じかどうかを確認する.
                BluetoothGattService service = gatt.getService(UUID.fromString(SERVICE_UUID));
                if ( service != null ) {
                    // 指定したUUIDを持つCharacteristicを確認する.
                    BluetoothGattCharacteristic characteristic = service.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID));
                    characteristic.setValue("ABCDEFG");
                    gatt.writeCharacteristic(characteristic);
                }
            }
        }

    }

    private static final int REQUEST_ENABLE_BT = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    @Override
    protected void onResume() {
        super.onResume();

        // デバイス選択用リスト
        list = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        view = (ListView)this.findViewById(R.id.listView);
        view.setAdapter(list);
        view.setOnItemClickListener(this);

        mDeviceList.clear();
        mDeviceName.clear();

        // BLEデバイス検索
        BluetoothManager bluetoothManager = (BluetoothManager)getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        if ( (mBluetoothAdapter == null) || !mBluetoothAdapter.isEnabled() ) {
            // Bluetoothデバイスが無効化されているので有効化する
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
        else {
            // Bluetoothデバイスが有効になっているのでスキャン開始
            mScanning = true;
            mBluetoothAdapter.startLeScan(MainActivity.this);
        }

    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
        final String name = device.getName();
        runOnUiThread(
                new Runnable() {
                    @Override
                    public void run() {
                        MainActivity.this.addDevice(device);
                    }
                });
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        if ( position < mDeviceList.size() ) {
            // 接続前にscanを停止
            if ( mScanning ) {
                mBluetoothAdapter.stopLeScan(this);
                mScanning = false;
            }

            // GATT serverへ接続
            mDeviceList.get(position).connectGatt(this, false, new MyGatt());
        }
    }

    private void addDevice(final BluetoothDevice device) {
        boolean isFound = false;
        for ( BluetoothDevice ldev : mDeviceList ) {
            if ( ldev.getAddress().equals(device.getAddress()) ) {
                isFound = true;
                break;
            }
        }

        if ( !isFound ) {
            mDeviceList.add(device);
            mDeviceName.add(device.getName());
            list.add(device.getName());
        }
    }

    /* UI object */
    private ArrayAdapter<String> list;
    private ListView view;

    /* device list */
    private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();
    private ArrayList<String> mDeviceName = new ArrayList<String>();

    private BluetoothAdapter mBluetoothAdapter;
    private boolean mScanning = false;
}
