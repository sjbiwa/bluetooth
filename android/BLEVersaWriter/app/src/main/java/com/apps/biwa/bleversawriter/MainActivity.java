package com.apps.biwa.bleversawriter;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.SpannableStringBuilder;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.UUID;

public class MainActivity extends AppCompatActivity
				implements	BluetoothAdapter.LeScanCallback,
							AdapterView.OnItemClickListener,
							View.OnClickListener {

	// 対象のサービスUUID.
	private static final String SERVICE_UUID = "07231001-B201-4924-8A0A-1F5EC5AA8FB9";
	// キャラクタリスティックUUID.
	private static final String CHARACTERISTIC_UUID = "07231002-B201-4924-8A0A-1F5EC5AA8FB9";

	private void resetMyGatt() {
		if ( mGatt != null ) {
			mGatt.close();
			mGatt = null;
			mService = null;
			mCharacteristic = null;
		}
	}
	private void uiConnectStatus(boolean isConn, String name) {
		if ( isConn ) {
			String str = "接続：" + name;
			mTextView.setText(str);
			Toast.makeText(MainActivity.this, "接続", Toast.LENGTH_SHORT).show();
		}
		else {
			mTextView.setText("切断");
			Toast.makeText(MainActivity.this, "切断", Toast.LENGTH_SHORT).show();
		}
	}

	class MyGatt extends BluetoothGattCallback {

		public MyGatt() {
			super();
		}

		@Override
		public void onConnectionStateChange(final BluetoothGatt gatt, int status, int newState) {
			super.onConnectionStateChange(gatt, status, newState);

			if ( newState == BluetoothProfile.STATE_CONNECTED ) {
				if ( mGatt == null ) {
					mGatt = gatt;
					gatt.discoverServices();
					runOnUiThread(
							new Runnable() {
								@Override
								public void run() {
									uiConnectStatus(true, gatt.getDevice().getName());
								}
							});

				}
				else if ( mGatt != gatt ) {
					gatt.close();
				}
			} else if ( newState == BluetoothProfile.STATE_DISCONNECTED ) {
				if ( mGatt == gatt ) {
					MainActivity.this.resetMyGatt();
				}
				runOnUiThread(
					new Runnable() {
						@Override
						public void run() {
							uiConnectStatus(false, null);
						}
					});
			}
		}

		@Override
		public void onServicesDiscovered(BluetoothGatt gatt, int status) {
			super.onServicesDiscovered(gatt, status);

			// Serviceが見つかったら実行.
			if ( status == BluetoothGatt.GATT_SUCCESS ) {
				// UUIDが同じかどうかを確認する.
				mService = gatt.getService(UUID.fromString(SERVICE_UUID));
				if ( mService != null ) {
					// 指定したUUIDを持つCharacteristicを確認する.
					mCharacteristic = mService.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID));
				}
			}
		}

	}


	@Override
	public void onClick(View view) {
		if ( view == btn1 ) {
			/* 送信ボタン */
			if ( (mGatt != null) && (mService != null) && (mCharacteristic != null) ) {
				EditText edit = (EditText)this.findViewById(R.id.editText);
				SpannableStringBuilder sb = (SpannableStringBuilder)edit.getText();
				String str = sb.toString();
				boolean stat = mCharacteristic.setValue(str);
				stat = mGatt.writeCharacteristic(mCharacteristic);
				Toast.makeText(this, "テキスト送信", Toast.LENGTH_SHORT).show();
			}
		}
		else if ( view == btn2 ) {
			/* 切断ボタン */
			if ( mGatt != null ) {
				resetMyGatt();
				uiConnectStatus(false, null);
			}
		}
	}

	private static final int REQUEST_ENABLE_BT = 1;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		// ボタン
		btn1 = (Button)this.findViewById(R.id.button);
		btn1.setOnClickListener(this);
		btn2 = (Button)this.findViewById(R.id.button2);
		btn2.setOnClickListener(this);
		// デバイス選択用リスト
		mList = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
		mView = (ListView)this.findViewById(R.id.listView);
		mView.setAdapter(mList);
		mView.setOnItemClickListener(this);
		// テキストビュー
		mTextView = (TextView)this.findViewById(R.id.textView);
		mTextView.setText("未接続：");
	}

	@Override
	protected void onResume() {
		super.onResume();

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
			mBluetoothAdapter.startLeScan(MainActivity.this);
		}

	}

	@Override
	protected void onPause() {
		super.onPause();

		resetMyGatt();
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
			// GATT serverへ接続
			mDeviceList.get(position).connectGatt(this, false, new MyGatt());
		}
	}

	private void addDevice(BluetoothDevice device) {
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
			mList.add(device.getName());
		}
	}

	/* UI object */
	private ArrayAdapter<String> mList;
	private ListView mView;
	private TextView mTextView;
	private Button btn1;
	private Button btn2;


	/* device list */
	private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();
	private ArrayList<String> mDeviceName = new ArrayList<String>();

	private BluetoothAdapter mBluetoothAdapter;

	private BluetoothGatt	mGatt = null; // 接続中のGATT
	private BluetoothGattService mService = null;
	private BluetoothGattCharacteristic mCharacteristic = null;
}
