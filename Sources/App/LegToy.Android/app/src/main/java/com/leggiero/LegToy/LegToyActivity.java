package com.leggiero.LegToy;

import android.util.Log;

import com.leggiero.Engine.LeggieroActivity;


// LegToy Activity
public class LegToyActivity extends LeggieroActivity {

	//////////////////////////////////////////////////////////////////////////////// Overriding Events

	@Override
	protected void onGameAppCreation() {
		Log.d("LegToy", "onGameAppCreation");
	}

	@Override
	protected void onGameAppCreated() {
		Log.d("LegToy", "onGameAppCreated");
	}

}
