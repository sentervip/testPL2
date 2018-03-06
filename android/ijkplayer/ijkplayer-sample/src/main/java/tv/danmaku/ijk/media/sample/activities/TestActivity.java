package tv.danmaku.ijk.media.sample.activities;

import android.app.Activity;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;

import java.io.File;

import tv.danmaku.ijk.media.sample.R;


/**
 * Created by Fred on 2018/2/25.
 */

public class TestActivity extends Activity {
    EditText urlEdit = null;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);
        urlEdit = (EditText) findViewById(R.id.testUrl);
        findViewById(R.id.playUrl).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String url = urlEdit.getText().toString();
                File f = new File(url);
                if(f.exists()){
                    VideoActivity.intentTo(TestActivity.this, f.getPath(), f.getName());
                }else{
                    Toast.makeText(TestActivity.this,"文件不存在",2000).show();
                }
            }
        });
        findViewById(R.id.camera).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //TODO 拍照
            }
        });
        findViewById(R.id.record).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //TODO 录像
            }
        });
    }
}
