/**
 * 该应用是个无聊游戏，只需要等待30s即可得到成功提示
 * 通过native层累加计数器，判断是否满足条件
 */
package com.example.gslab.ihook;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.view.View;
import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    public TextView resultTextView;         //正中间结果文本框
    public Button loadEvilModuleButton;     //该button是给内存读写用

    /**
     * 每秒自动更新结果文本框，调用native的UpdateResult函数获取tip串
     */
    void updateText() {
        resultTextView.postDelayed(new Runnable() {
            @Override
            public void run() {
                resultTextView.setText(UpdateResult());
                updateText();
            }
        }, 1000);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //加载应用自身的功能so
        System.loadLibrary("IHook");

        resultTextView = (TextView)findViewById(R.id.textViewResult);
        loadEvilModuleButton = (Button)findViewById(R.id.loadButton);
        loadEvilModuleButton.setOnClickListener(this);

        //开始刷新结果文本框
        resultTextView.setText("Hi");
        updateText();
    }

    /**
     * 进程内内存读写测试入口，加载恶意模块
     * @param v
     */
    @Override
    public void onClick(View v) {
        System.loadLibrary("MemOperation");
        loadEvilModuleButton.setEnabled(false);
    }

    //natvie功能函数，更新结果文本框字符串
    public static native String UpdateResult();
}
