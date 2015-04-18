using AHRS;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace Comunication_App
{
    public partial class Form1 : Form
    {
        public int i;



        public Form1()
        {
            InitializeComponent();

            i = 0;
            //Abrir el puerto serie al inicio.
            try
            {
                //serialPort1.Open();
                comPorts_comBox.DataSource = SerialPort.GetPortNames();
                
            }
            catch
            {

            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            //Cerrar el puerto serie al cerrar el formulario.
            serialPort1.Close();
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            Byte b = 0;
            //Mientras haya Bytes
            while (serialPort1.BytesToRead > 0)
            {
                b = (Byte)serialPort1.ReadByte();
                //Procesar el dato recibido
                RxByteStateMachine(b);


            }
        }

        public int RxStatus, RxLen, RxIdx;
        public Byte[] RxBuff;

        public void RxByteStateMachine(Byte RxByte)
        {
            switch (RxStatus)
            {
                case 0:     //Inicio del paquete
                    if (RxByte == 0x7E) //Si se recibe el inicio de trama
                    {
                        RxStatus = 1;   //Estado de longitud
                    }
                    break;
                case 1:     //Longitud
                    RxLen = RxByte;
                    RxIdx = 0;
                    RxBuff = new Byte[RxLen];
                    RxStatus = 2;
                    break;
                case 2:     //Recepción de datos
                    RxBuff[RxIdx] = RxByte;
                    RxIdx++;
                    if (RxIdx == RxLen)
                    {
                        RxStatus = 3;
                    }
                    break;
                case 3:     //Fin del paquete
                    if (RxByte == 0x0D || RxByte == 0xAD || RxByte == 0x0F || RxByte == 0x0A) //Si se recibe el fin de trama
                    {   //Descodificar el paquete
                        ParsePacktet(RxBuff, RxByte);

                    }

                    RxStatus = 0;   //Vuela al estado inicial
                    break;
                default:
                    break;
            }
        }


        SaveFileDialog saveFileDialog1 = new SaveFileDialog();
        MadgwickAHRS updatevalues = new MadgwickAHRS(395.0f, 6.0f);
        float gfx, gfy, gfz, gdfx, gdfy, gdfz, afx, afy, afz, mfx, mfy, mfz, timervalue;
        float[] q = new float[4];
        float pitch = 0, roll = 0, yaw = 0;
        float longitud=0;
         float  latitud=0;

        float pitchb = 0, rollb = 0, yawb = 0;
        float[] ypr = new float[4];
        float[] angles = new float[4];
        float differencepitch, differenceroll, differenceyaw;

        List<float> smooth = new List<float>() { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        float alpha = 0.5f;
        int nmin = 0;
        int nmax = 400;
        private void Form1_Paint(object sender, PaintEventArgs e)
        {

            textBox2.Clear();
            textBox3.Clear();
            textBox4.Clear();
            textBox6.Clear();
            textBox7.Clear();
            textBox8.Clear();


            textBox8.AppendText(updatevalues.Beta.ToString("0.00"));


            textBox2.AppendText("x del acelerometro: " + afx.ToString("0.0000") + "\r\n");
            textBox2.AppendText("y del acelerometro: " + afy.ToString("0.0000") + "\r\n");
            textBox2.AppendText("z del acelerometro: " + afz.ToString("0.0000") + "\r\n");


            textBox3.AppendText("x del giroscopo" + gfx.ToString("0.0000") + "\r\n");
            textBox3.AppendText("y del giroscopo" + gfy.ToString("0.0000") + "\r\n");
            textBox3.AppendText("z del giroscopo" + gfz.ToString("0.0000") + "\r\n");


            textBox4.AppendText("x del magenetometro" + mfx.ToString("0.0000") + "\r\n");
            textBox4.AppendText("y del magenetometro" + mfy.ToString("0.0000") + "\r\n");
            textBox4.AppendText("z del magenetometro" + mfz.ToString("0.0000") + "\r\n");


            smooth[0] = (gfx * alpha + (1 - alpha) * smooth[0]);
            smooth[1] = (gfy * alpha + (1 - alpha) * smooth[1]);
            smooth[2] = (gfz * alpha + (1 - alpha) * smooth[2]);
            smooth[3] = (afx * alpha + (1 - alpha) * smooth[3]);
            smooth[4] = (afy * alpha + (1 - alpha) * smooth[4]);
            smooth[5] = (afz * alpha + (1 - alpha) * smooth[5]);
            smooth[6] = (mfx * alpha + (1 - alpha) * smooth[6]);
            smooth[7] = (mfy * alpha + (1 - alpha) * smooth[7]);
            smooth[8] = (mfz * alpha + (1 - alpha) * smooth[8]);


            gdfx = gfx * ((float)Math.PI / 180.0f);
            gdfy = gfx * ((float)Math.PI / 180.0f);
            gdfz = gfx * ((float)Math.PI / 180.0f);

            //  updatevalues.Update(gdfx, gdfy, gdfz, smooth[3], smooth[4], smooth[5], smooth[6], smooth[7], smooth[8]);//con filtro
            updatevalues.Update(gdfx, gdfy, gdfz, afx, afy, afz, mfx, mfy, mfz);//Sin filtro de medias
            Quaterniones(q, updatevalues);

            textBox7.AppendText("q0:" + updatevalues.Quaternion[0] + "\r\n");
            textBox7.AppendText("q1:" + updatevalues.Quaternion[1] + "\r\n");
            textBox7.AppendText("q2:" + updatevalues.Quaternion[2] + "\r\n");
            textBox7.AppendText("q3:" + updatevalues.Quaternion[3] + "\r\n");

            /*textBox5.AppendText("timer: " + timervalue.ToString()+"us" + "\r\n");*/
            textBox5.AppendText("timer: " + timervalue.ToString() + "us" + "\r\n");

            getYawPitchRollRad(ypr, q);
            pitchb = -(float)Math.Atan(afx / ((afx * afx) + (afz * afz))) ;// ypr[1];
            rollb = -(float)Math.Atan(afy / ((afz * afz) + (afy * afy)));//ypr[2];
            yawb = ypr[0];

            differencepitch = pitch - pitchb;
            differenceroll = roll - rollb;
            differenceyaw = yaw - yawb;



            textBox6.AppendText("pitch:" + pitch + "\r\n");
            textBox6.AppendText("roll:" + roll + "\r\n");
            textBox6.AppendText("yaw:" + yaw + "\r\n");
            textBox6.AppendText("pitchb:" + pitchb + "\r\n");
            textBox6.AppendText("rollb:" + rollb + "\r\n");
            textBox6.AppendText("yawb:" + yawb + "\r\n");
            textBox6.AppendText("Longitud" + longitud + "\r\n");
            textBox6.AppendText("Latitud" + latitud + "\r\n");
            //Datos aplicacion c#
            chart1.ChartAreas[0].AxisX.Minimum = nmin;
            chart1.ChartAreas[0].AxisX.Maximum = nmax;
            chart1.ChartAreas[0].AxisY.Maximum = 2;
            chart1.ChartAreas[0].AxisY.Minimum = -2;
            chart2.ChartAreas[0].AxisX.Minimum = nmin;
            chart2.ChartAreas[0].AxisX.Maximum = nmax;
            chart2.ChartAreas[0].AxisY.Maximum = 4;
            chart3.ChartAreas[0].AxisX.Minimum = nmin;
            chart3.ChartAreas[0].AxisX.Maximum = nmax;
            chart3.ChartAreas[0].AxisY.Minimum = -2;
            chart3.ChartAreas[0].AxisY.Maximum = 2;

            chart1.Series["Pitchb"].ChartType = SeriesChartType.FastLine;
            chart1.Series["Pitchb"].Color = Color.Black;
            chart1.Series["Pitchb"].Points.AddXY(i, pitchb);


          /*  chart2.Series["Yawb"].ChartType = SeriesChartType.FastLine;
            chart2.Series["Yawb"].Color = Color.Black;
            chart2.Series["Yawb"].Points.AddXY(i, yawb);*/

            chart3.Series["Rollb"].ChartType = SeriesChartType.FastLine;
            chart3.Series["Rollb"].Color = Color.Black;
            chart3.Series["Rollb"].Points.AddXY(i, rollb);

            //Datos Microcontrolador
            chart1.Series["Pitch"].ChartType = SeriesChartType.FastLine;
            chart1.Series["Pitch"].Color = Color.Blue;
            chart1.Series["Pitch"].Points.AddXY(i, pitch);

            chart2.Series["Yaw"].ChartType = SeriesChartType.FastLine;
            chart2.Series["Yaw"].Color = Color.Orange;
            chart2.Series["Yaw"].Points.AddXY(i, yaw);

            chart3.Series["Roll"].ChartType = SeriesChartType.FastLine;
            chart3.Series["Roll"].Color = Color.Green;
            chart3.Series["Roll"].Points.AddXY(i, roll);

            //Diferencia
         /*   chart1.Series["Error"].ChartType = SeriesChartType.FastLine;
            chart1.Series["Error"].Color = Color.Red;
            chart1.Series["Error"].Points.AddXY(i, differencepitch);



            chart2.Series["Error"].ChartType = SeriesChartType.FastLine;
            chart2.Series["Error"].Color = Color.Red;
            chart2.Series["Error"].Points.AddXY(i, differenceyaw);

            chart3.Series["Error"].ChartType = SeriesChartType.FastLine;
            chart3.Series["Error"].Color = Color.Red;
            chart3.Series["Error"].Points.AddXY(i, differenceroll);*/
            i++;
            if (i == nmax)
            {
                nmin = nmax / 2;
                nmax =


                    (nmax + nmin);
            }
        }

        private void Quaterniones(float[] q, MadgwickAHRS updatevalues)
        {
            q[0] = updatevalues.Quaternion[0];
            q[1] = updatevalues.Quaternion[1];
            q[2] = updatevalues.Quaternion[2];
            q[3] = updatevalues.Quaternion[3];
        }
        void getYawPitchRollRad(float[] ypr, float[] q)
        {

            float gx, gy, gz;

            gx = 2 * (q[1] * q[3] - q[0] * q[2]);
            gy = 2 * (q[0] * q[1] + q[2] * q[3]);
            gz = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];

            ypr[0] = -(float)Math.Atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0] * q[0] + 2 * q[1] * q[1] - 1);
            ypr[1] = (float)Math.Atan(gx / (float)Math.Sqrt(gy * gy + gz * gz));
            ypr[2] = (float)Math.Atan(gy / (float)Math.Sqrt(gx * gx + gz * gz));
        }

        void geteulerangles(float[] q, float[] angles)
        {

            angles[0] = (float)Math.Atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0] * q[0] + 2 * q[1] * q[1] - 1) * 180 / (float)Math.PI; // psi
            angles[1] = (float)-Math.Asin(2 * q[1] * q[3] + 2 * q[0] * q[2]) * 180 / (float)Math.PI; // thet
            angles[2] = (float)Math.Atan2(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1) * 180 / (float)Math.PI; // phi
        }


        public void ParsePacktet(Byte[] Packet, int Rxbyte)
        {
            if (Rxbyte == 0x0D)
            {
                //Datos para la aplicacion

                afx = System.BitConverter.ToSingle(Packet, 0);
                afy = System.BitConverter.ToSingle(Packet, 4);
                afz = System.BitConverter.ToSingle(Packet, 8);

                mfx = System.BitConverter.ToSingle(Packet, 12);
                mfy = System.BitConverter.ToSingle(Packet, 16);
                mfz = System.BitConverter.ToSingle(Packet, 20);


                gfx = System.BitConverter.ToSingle(Packet, 24);
                gfy = System.BitConverter.ToSingle(Packet, 28);
                gfz = System.BitConverter.ToSingle(Packet, 32);


                timervalue = System.BitConverter.ToSingle(Packet, 36);


            }
            else if (Rxbyte == 0xAD)
            {
                //datos del microcontrolador
                pitch = System.BitConverter.ToSingle(Packet, 0);
                roll = System.BitConverter.ToSingle(Packet, 4);
                yaw = System.BitConverter.ToSingle(Packet, 8);

                timervalue = System.BitConverter.ToSingle(Packet, 12);
                /*latitud = (float)System.BitConverter.ToInt16(Packet, 14)/1000;
                longitud = (float)System.BitConverter.ToInt16(Packet, 16)/1000;*/



            }
            else if (Rxbyte == 0x0F)
            {
                //Datos del logging
                float rolldata = ((float)BitConverter.ToInt16(Packet, 0)) / 1000.0f;
                float pitchdata = ((float)BitConverter.ToInt16(Packet, 2)) / 1000.0f;
                float yawdata = ((float)BitConverter.ToInt16(Packet, 4)) / 1000.0f;

                float ax = ((float)BitConverter.ToInt16(Packet, 6)) / 1000.0f;
                float ay = ((float)BitConverter.ToInt16(Packet, 8)) / 1000.0f;
                float az = ((float)BitConverter.ToInt16(Packet, 10)) / 1000.0f;

                float gx = ((float)BitConverter.ToInt16(Packet, 12)) / 1000.0f;
                float gy = ((float)BitConverter.ToInt16(Packet, 14)) / 1000.0f;
                float gz = ((float)BitConverter.ToInt16(Packet, 16)) / 1000.0f;

                float mx = ((float)BitConverter.ToInt16(Packet, 18)) / 1000.0f;
                float my = ((float)BitConverter.ToInt16(Packet, 20)) / 1000.0f;
                float mz = ((float)BitConverter.ToInt16(Packet, 22)) / 1000.0f;
                
                
               float timersample = ((float)BitConverter.ToInt16(Packet, 24));
          

             //  int lat = ((float)((float)BitConverter.ToInt16(Packet, 26)) / 10000.0f);
            //   float lon = ((float)((float)BitConverter.ToInt16(Packet, 28)) / 10000.0f);
                File.AppendAllText(saveFileDialog1.FileName, rolldata.ToString() + ";" + pitchdata.ToString() + ";" + yawdata.ToString() + ";" + ax.ToString() + ";" + ay.ToString() + ";" + az.ToString() + ";" + gx.ToString() + ";" + gy.ToString() + ";" + gz.ToString() + ";" + mx.ToString() + ";" + my.ToString() + ";" + mz.ToString() + ";" + timersample.ToString() +";"+longitud.ToString()+";"+latitud.ToString() +"\n", Encoding.UTF8);


            }
            else
            {
                //ACK del loging
                MessageBox.Show("Datos enviados de forma correcta");
            }

            this.Invalidate();
        }


        private void button1_Click(object sender, EventArgs e)
        {
            /*  textBox1.Text = "Botón Pulsado!: " + i;*/
            i++;
            if (i == 4)
            {
                i = 0;
            }
            //TxCmd('L', (byte)i);
        }

        public void TxCmd(char cmd)
        {
            byte[] buffer = new Byte[1];
            buffer[0] = (byte)cmd;
            try
            {
                serialPort1.Write(buffer, 0, 1);
            }
            catch
            {
            }
        }



        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            foreach (var series in chart1.Series)
            {
                series.Points.Clear();
            }
            foreach (var series in chart2.Series)
            {
                series.Points.Clear();
            }
            foreach (var series in chart3.Series)
            {
                series.Points.Clear();
            }


            i = 0;
            nmin = 0;
            nmax = 200;
            chart1.ChartAreas[0].AxisX.Minimum = nmin;
            chart1.ChartAreas[0].AxisX.Maximum = nmax;
            chart1.ChartAreas[0].AxisY.Maximum = 3;
            chart2.ChartAreas[0].AxisX.Minimum = nmin;
            chart2.ChartAreas[0].AxisX.Maximum = nmax;
            chart2.ChartAreas[0].AxisY.Maximum = 3;
            chart3.ChartAreas[0].AxisX.Minimum = nmin;
            chart3.ChartAreas[0].AxisX.Maximum = nmax;
            chart3.ChartAreas[0].AxisY.Maximum = 3;

        }



        private void button7_Click(object sender, EventArgs e)
        {
            foreach (var series in chart1.Series)
            {
                series.Points.Clear();
            }
            foreach (var series in chart2.Series)
            {
                series.Points.Clear();
            }
            foreach (var series in chart3.Series)
            {
                series.Points.Clear();
            }
            i = 0;
            i = 0;
            nmin = 0;
            nmax = 200;
            chart1.ChartAreas[0].AxisX.Minimum = nmin;
            chart1.ChartAreas[0].AxisX.Maximum = nmax;
            chart1.ChartAreas[0].AxisY.Maximum = 3;
            chart2.ChartAreas[0].AxisX.Minimum = nmin;
            chart2.ChartAreas[0].AxisX.Maximum = nmax;
            chart2.ChartAreas[0].AxisY.Maximum = 3;
            chart3.ChartAreas[0].AxisX.Minimum = nmin;
            chart3.ChartAreas[0].AxisX.Maximum = nmax;
            chart3.ChartAreas[0].AxisY.Maximum = 3;
            updatevalues = new MadgwickAHRS(395.5f, float.Parse(textBox8.Text));

        }

        private void button4_Click(object sender, EventArgs e)
        {
            updatevalues.Beta = updatevalues.Beta - 0.1f;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            updatevalues.Beta = updatevalues.Beta + 0.1f;
        }
        private void reiniciograficas()
        {
            i = 0;
            chart1.ChartAreas[0].AxisX.Minimum = nmin;
            chart1.ChartAreas[0].AxisX.Maximum = nmax;
            chart1.ChartAreas[0].AxisY.Maximum = 4;
            chart2.ChartAreas[0].AxisX.Minimum = nmin;
            chart2.ChartAreas[0].AxisX.Maximum = nmax;
            chart2.ChartAreas[0].AxisY.Maximum = 4;
            chart3.ChartAreas[0].AxisX.Minimum = nmin;
            chart3.ChartAreas[0].AxisX.Maximum = nmax;
            chart3.ChartAreas[0].AxisY.Maximum = 3;
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "CSV Files (*.csv)|*.csv";

            saveFileDialog1.ShowDialog();


            if (saveFileDialog1.FileName != "")
            {

                System.IO.FileStream fs =
                   (System.IO.FileStream)saveFileDialog1.OpenFile();

                fs.Close();
                File.WriteAllText(saveFileDialog1.FileName, "Roll;Pitch;Yaw;Ax;Ay;Az;Gx;Gy;Gz;Mx;My;Mz;SampleTime;Longitud;Latitud\n", Encoding.UTF8);
                TxCmd('L');
            }

        }

        private void button5_Click(object sender, EventArgs e)
        {
            TxCmd('C');
        }

        private void button6_Click(object sender, EventArgs e)
        {
            String selection = comboBox1.Text;
            switch (selection)
            {
                case "2000":
                    TxCmd('A');
                    break;
                case "500":
                    TxCmd('B');
                    break;
                case "250":
                    TxCmd('D');
                    break;
                default: TxCmd('A');
                    break;
            }
        }


        public void Confdisp(byte value)
        {
            byte[] buffer = new Byte[2];


            buffer[0] = value;


            try
            {
                serialPort1.Write(buffer, 0, 2);
            }
            catch
            {



            }

        }

        private void button8_Click(object sender, EventArgs e)
        {
            String selection = comboBox2.Text;
            switch (selection)
            {
                case "2":
                    TxCmd('a');
                    break;
                case "4":
                    TxCmd('b');
                    break;
                case "8":
                    TxCmd('d');
                    break;
                default: TxCmd('a');
                    break;
            }
        }

        private void button9_Click(object sender, EventArgs e)
        {
            String selection = comboBox3.Text;
            switch (selection)
            {
                case "1.3":
                    TxCmd('1');
                    break;
                case "1.9":
                    TxCmd('2');
                    break;
                case "2.5":
                    TxCmd('3');
                    break;
                default: TxCmd('1');
                    break;
            }
        }

                          

        private void comPorts_comBox_SelectedValueChanged(object sender, EventArgs e)

        {

            try {
                serialPort1.Close();
                serialPort1.PortName = comPorts_comBox.Text;
                serialPort1.Open();
            }
            catch { 
            }
          
        
           
        }

        /*private void button10_Click(object sender, EventArgs e)
        {
            this.serialPort1.PortName = "COM" + Convert.ToInt16(textBox1.Text);
        }*/





    }



}

