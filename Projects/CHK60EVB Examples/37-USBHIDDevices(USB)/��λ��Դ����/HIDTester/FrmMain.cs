using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using  System.Globalization;
using HID;
namespace HIDTester
{
    public partial class FrmMain : Form
    {
        public FrmMain()
        {
            InitializeComponent();
            myhid .DataReceived +=new EventHandler<HID.report>(myhid_DataReceived); //订阅DataRec事件
            myhid .DeviceRemoved +=new EventHandler(myhid_DeviceRemoved);
        }
        HID.Hid myhid = new HID.Hid();
        Boolean IsDataReceived = false;
        Byte[] RecDataBuffer = new byte[90];
        private void GetVendorAndProductIDsFromTextBoxes(ref UInt16 myVendorID, ref UInt16 myProductID)
        {
            try
            {
                myVendorID = UInt16.Parse(txtVendorID.Text, NumberStyles.AllowHexSpecifier);
                myProductID = UInt16.Parse(txtProductID.Text, NumberStyles.AllowHexSpecifier);
            }
            catch (Exception ex)
            {
                //DisplayException(this.Name, ex);
                throw;
            }
        }      
        private void FrmMain_Load(object sender, EventArgs e)
        {
            lstResults.Items.Clear();
            this.Text = "超核电子 K60开发板 自定义HID设备测试";
        }

        private void cmdFindDevice_Click(object sender, EventArgs e)
        {
            List<string> deviceList = new List<string>();
            HID.Hid.HID_RETURN HidResult;
            UInt16  myProductID = 0;
            UInt16 myVendorID = 0;
            if (myhid.Opened == false)
            {
                //获得VID和PID
                GetVendorAndProductIDsFromTextBoxes(ref myVendorID, ref  myProductID);
                //尝试获得 serial
                HidResult = HID.Hid.GetDeviceSerialList(myVendorID, myProductID, ref deviceList);
                lstResults.Items.Add("找到" + deviceList.Count + "个设备");
                if (deviceList.Count == 1)
                {
                    //打开设备
                    HidResult = myhid.OpenDevice(myVendorID, myProductID, deviceList[0]);
                    if (HidResult == Hid.HID_RETURN.SUCCESS)
                    {
                        lstResults .Items.Add ("设备打开成功");
                        lstResults.Items.Add("输出报告长度:" + myhid.OutputReportLength);
                        lstResults.Items.Add("输入报告长度" + myhid.InputReportLength);
                        timer1.Enabled = true;
                        groupBox1.Enabled = true;
                        cmdFindDevice.Enabled = false;
                    }
                    else
                    {
                        lstResults.Items.Add("设备打开失败");
                    }
                }
            }
        }
        //数据到达事件
        protected void myhid_DataReceived(object sender, report e)
        {
            IsDataReceived = true;
            RecDataBuffer = e.reportBuff;
        }
       //设备移除事件
        protected void myhid_DeviceRemoved(object sender, EventArgs e)
        {
            
        }
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (myhid.Opened == true)
            {
                this.Text = "设备已连接";
                cmdFindDevice.Enabled = false;
                groupBox1.Enabled = true;
            }
            else
            {
                this.Text = "设备已断开";
                cmdFindDevice.Enabled = true;
                groupBox1.Enabled = false ;
            }
            if (IsDataReceived == true) //数据接收到了
            {
                uint i;
                IsDataReceived = false;
                string RecStr = "接收到的数据:";
                for (i = 0; i < myhid.InputReportLength-1; i++)
                {
                    RecStr += String.Format("{0:X2} ", RecDataBuffer[i]);
                }
                lstResults.Items.Add(RecStr);
                //更改 按键label的状态
                if (RecDataBuffer[0] == 0x01)
                {
                    this.lab_KEY1.ForeColor = Color.Gray;
                    this.lab_KEY1.Text = "KEY1:UNPRESSED";
                }
                else if (RecDataBuffer[0] == 0x00)
                {
                    this.lab_KEY1.ForeColor = Color.Red;
                    this.lab_KEY1.Text = "KEY1:PRESSED";
                }
                if (RecDataBuffer[1] == 0x01)
                {
                    this.lab_KEY2.ForeColor = Color.Gray;
                    this.lab_KEY2.Text = "KEY2:UNPRESSED";
                }
                else if (RecDataBuffer[1] == 0x00)
                {
                    this.lab_KEY2.ForeColor = Color.Red;
                    this.lab_KEY2.Text = "KEY2:PRESSED";
                }
            }
        }
        private void btnClear_Click(object sender, EventArgs e)
        {
            lstResults.Items.Clear();
        }
        private void radioButton1_Click(object sender, EventArgs e)
        {
            Byte[] data = new byte [8];
            report r = new report(0, data);
            data[0] = 0x01;
            myhid.Write(r);
        }
        private void radioButton2_Click(object sender, EventArgs e)
        {
            Byte[] data = new byte[8];
            report r = new report(0, data);
            data[0] = 0x02;
            myhid.Write(r);
        }
    }
}
