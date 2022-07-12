using AForge.Imaging.Filters;
using AForge.Video;
using AForge.Video.DirectShow;
using AForge.Video.FFMPEG;
using GMap.NET.MapProviders;
using System;
using System.IO;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Globalization;
using Excel = Microsoft.Office.Interop.Excel;

namespace Player
{
    public partial class MainForm : Form
    {
        int maksm = 5, minm = 0;  // chart values
        int i = 0;
        private Stopwatch stopWatch = null;
        double latitude,longitude;
        string timer17label=" ";
       
        UdpClient Client = new UdpClient(52000);
        string mesaj = "";

        Boolean videoAaktarım = false;
        Boolean manueltahrik = false;
        Boolean manuelayırma = false;
        Bitmap image_1;
        public MainForm( )
        {
            CheckForIllegalCrossThreadCalls = false;
            InitializeComponent( );
        }

        private void MainForm_Load(object sender, EventArgs e)  // FORM LOAD
        {
            string[] title = {"Takım No","Paket Numarası","Gönderme Saati","Basınç 1","Basınç2","Yükseklik1","Yükseklik2","İrtifa Farkı","İniş Hızı","Sıcaklık","Pil Gerilimi",
            "Gps1 Latitude","Gps1 Longitude","Gps1 Altitude","Gps2 Latitude","Gps2 Longitude","Gps2 Altitude","Uydu Statüsü","Pitch","Roll","Yaw","Dönüş Sayısı",
            "Video Aktarım Bilgisi"};
            dataGridView1.Rows.Add(title);

            this.gMapControl1.DragButton = MouseButtons.Left;//Left and drag the map
            this.gMapControl1.MapProvider = GMapProviders.GoogleMap;//Specify the map type
            this.gMapControl1.MinZoom = 1; //minimum ratio
            this.gMapControl1.MaxZoom = 100; //Maximum ratio
            this.gMapControl1.Zoom = 16; //current ratio
            gMapControl1.ShowCenter = true;
            this.gMapControl2.DragButton = MouseButtons.Left;//Left and drag the map
            this.gMapControl2.MapProvider = GMapProviders.GoogleMap;//Specify the map type
            this.gMapControl2.MinZoom = 1; //minimum ratio
            this.gMapControl2.MaxZoom = 100; //Maximum ratio
            this.gMapControl2.Zoom = 16; //current ratio
            gMapControl2.ShowCenter = true;

            timer1.Enabled = true;
            image_1 = (Bitmap)pictureBox5.Image;    
            backgroundWorker1.RunWorkerAsync();
        }

        /* //////////////////////////////////////////////WEBCAM START//////////////////////////////////////////////////// */
        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            if(backgroundWorker1.IsBusy != true)
            {
                //-----------------RECORD
                VideoCaptureDevices = new FilterInfoCollection(FilterCategory.VideoInputDevice);
                captureDevice = new VideoCaptureDeviceForm();
                //-----------------RECORD
                timer1.Start();
            }
        }

        private void MainForm_FormClosing( object sender, FormClosingEventArgs e )
        {
            CloseCurrentVideoSource( );
        }

        // "Exit" menu item clicked
        private void exitToolStripMenuItem_Click( object sender, EventArgs e )
        {
            this.Close( );
        }

        // Capture 1st display in the system
        private void capture1stDisplayToolStripMenuItem_Click( object sender, EventArgs e )
        {
            OpenVideoSource( new ScreenCaptureStream( Screen.AllScreens[0].Bounds, 100 ) );
        }

        // Open video source
        private void OpenVideoSource( IVideoSource source )
        {
            // set busy cursor
            this.Cursor = Cursors.WaitCursor;

            // stop current video source
            CloseCurrentVideoSource( );

            // start new video source
            videoSourcePlayer.VideoSource = source;
            videoSourcePlayer.Start( );

            // reset stop watch
            stopWatch = null;

            // start timer
            timer.Start( );

            this.Cursor = Cursors.Default;
        }

        // Close video source if it is running
        private void CloseCurrentVideoSource( )
        {
            if ( videoSourcePlayer.VideoSource != null )
            {
                videoSourcePlayer.SignalToStop( );

                // wait ~ 3 seconds
                for ( int i = 0; i < 30; i++ )
                {
                    if ( !videoSourcePlayer.IsRunning )
                        break;
                    System.Threading.Thread.Sleep( 100 );
                }

                if ( videoSourcePlayer.IsRunning )
                {
                    videoSourcePlayer.Stop( );
                }

                videoSourcePlayer.VideoSource = null;
            }
        }

        private void timer_Tick( object sender, EventArgs e )
        {
            IVideoSource videoSource = videoSourcePlayer.VideoSource;

            if ( videoSource != null )
            {
                // get number of frames since the last timer tick
                int framesReceived = videoSource.FramesReceived;

                if ( stopWatch == null )
                {
                    stopWatch = new Stopwatch( );
                    stopWatch.Start( );
                }
                else
                {
                    stopWatch.Stop( );
                    stopWatch.Reset( );
                    stopWatch.Start( );
                }
            }
        }

        //-----------------RECORD
        private FilterInfoCollection VideoCaptureDevices;

        private VideoCaptureDevice FinalVideo = null;
        private VideoCaptureDeviceForm captureDevice;

        private Bitmap video;
        
        //private AVIWriter AVIwriter = new AVIWriter();
        public VideoFileWriter FileWriter = new VideoFileWriter();
        private SaveFileDialog saveAvi;
        //-----------------RECORD
        private void ButtonRecSave_Click(object sender, EventArgs e)
        {
            if(buttonRecStop.Text == "Stop Camera")
            { 
                saveAvi = new SaveFileDialog();
                saveAvi.Filter = "Avi Files (*.avi)|*.avi";
                if (saveAvi.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    int h = captureDevice.VideoDevice.VideoResolution.FrameSize.Height;
                    int w = captureDevice.VideoDevice.VideoResolution.FrameSize.Width;
                    FileWriter.Open(saveAvi.FileName, w, h, 25, VideoCodec.Default, 5000000);
                    FileWriter.WriteVideoFrame(video);

                    buttonRecStop.Text = "Stop Record";
                }
            }
        }

        private void ButtonRecStart_Click(object sender, EventArgs e)
        {
            captureDevice = new VideoCaptureDeviceForm();

            if (captureDevice.ShowDialog(this) == DialogResult.OK)
            {
                // create video source
                FinalVideo = captureDevice.VideoDevice;

                // open it
                OpenVideoSource(FinalVideo);
                FinalVideo.NewFrame += new NewFrameEventHandler(FinalVideo_NewFrame);
                FinalVideo.Start();
            }
        }

        void FinalVideo_NewFrame(object sender, NewFrameEventArgs eventArgs)
        {
            Bitmap oldVideo = video;

            if (buttonRecStop.Text == "Stop Record")
            {
                
                video = (Bitmap)eventArgs.Frame.Clone();

                if (pictureBox1.Image != null)
                {
                    pictureBox1.Image.Dispose();
                }
                pictureBox1.Image = video;

                pictureBox1.Image = (Bitmap)eventArgs.Frame.Clone();
                //AVIwriter.Quality = 0;
                FileWriter.WriteVideoFrame(video);
                if(oldVideo != null )
                    oldVideo.Dispose();

                // AVIwriter.AddFrame(video);
            }
            else //Stop
            {
                if (pictureBox1.Image != null)
                {
                    pictureBox1.Image.Dispose();
                }
                video = (Bitmap)eventArgs.Frame.Clone();
                pictureBox1.Image = (Bitmap)eventArgs.Frame.Clone();
                if( oldVideo != null)
                oldVideo.Dispose();
            }
        }

        private void ButtonRecStop_Click(object sender, EventArgs e)
        {
            if (buttonRecStop.Text == "Stop Record")
            {
                buttonRecStop.Text = "Stop Camera";
                if (FinalVideo == null)
                { return; }
                if (FinalVideo.IsRunning)
                {
                    //this.FinalVideo.Stop();
                    FileWriter.Close();
                    //this.AVIwriter.Close();
                    pictureBox1.Image = null;
                }
            }
            else
            {
                this.FinalVideo.Stop();
                FileWriter.Close();
                //this.AVIwriter.Close();
                pictureBox1.Image = null;
            }
        }
        /* //////////////////////////////////////////////WEBCAM END//////////////////////////////////////////////////// */


        /* //////////////////////////////////////////////PortReading START//////////////////////////////////////////////////// */
        string[] words;
        private void displayData_event(object sender, EventArgs e)  // Veriler split edilip ListView e ekleniyor
        {
            string[] veriler = new string[24];
            string phrase = mesaj;
            words = phrase.Split(',');
            int p = 1;
           
            foreach (var word in words)
            {
                veriler[p-1] = word;
                p++;

                if (p % 25 == 0)
                {
                    dataGridView1.Rows.Add(veriler);
                   
                    Array.Clear(veriler, 0, 23);
                    mesaj.Remove(0,mesaj.Length);

                    i++;
                }

                if (i==1)
                {
                    timer2.Enabled = true;
                    timer4.Enabled = true;
                    timer5.Enabled = true;
                    timer6.Enabled = true;
                    timer7.Enabled = true;
                    timer8.Enabled = true;
                    timer9.Enabled = true;
                    timer11.Enabled = true;
                    timer12.Enabled = true;
                    timer13.Enabled = true;
                    timer14.Enabled = true;
                    timer15.Enabled = true;
                    timer16.Enabled = true;
                    timer17.Enabled = true;
                }         
            }
        }
        private void addData()  // Veriler split edilip ListView e ekleniyor
        {
            string[] veriler = new string[23];
            string phrase = mesaj;
            words = phrase.Split(',');
            int p = 1;
            timer17.Enabled = true;

            foreach (var word in words)
            {
                veriler[p - 1] = word;
                p++;

                if (p % 24 == 0)
                {
                    dataGridView1.Rows.Add(veriler);
                    timer17label = veriler[7];
                   
                    Array.Clear(veriler, 0, 23);
                    mesaj.Remove(0, mesaj.Length);

                    i++;
                }

                if (i == 1)
                {
                    timer2.Enabled = true;
                    timer4.Enabled = true;
                    timer5.Enabled = true;
                    timer6.Enabled = true;
                    timer7.Enabled = true;
                    timer8.Enabled = true;
                    timer9.Enabled = true;
                    timer11.Enabled = true;
                    timer12.Enabled = true;
                    timer13.Enabled = true;
                    timer14.Enabled = true;
                    timer15.Enabled = true;
                    timer16.Enabled = true;
                    timer17.Enabled = true;
                }
            }
        }

        /* //////////////////////////////////////////////PortReading End//////////////////////////////////////////////////// */

        bool export_dgw_excel_1(DataGridView dgw)
        {
            bool durum = false;
            try
            {
                dgw.SelectAll();
                DataObject dataObj = dgw.GetClipboardContent();
                if (dataObj != null)
                    Clipboard.SetDataObject(dataObj);
                Excel.Application xlexcel;
                Excel.Workbook xlWorkBook;
                Excel.Worksheet xlWorkSheet;
                object misValue = System.Reflection.Missing.Value;
                xlexcel = new Excel.Application();
                xlexcel.Visible = true;
                xlWorkBook = xlexcel.Workbooks.Add(misValue);
                xlWorkSheet = (Excel.Worksheet)xlWorkBook.Worksheets.get_Item(1);
                Excel.Range CR = (Excel.Range)xlWorkSheet.Cells[1, 1];
                CR.Select();
                xlWorkSheet.PasteSpecial(CR, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing, true);
                durum = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("DataGrid Verileri Aktarılamadı : " + ex.Message);
            }
            return durum;
        }

        private void button9_Click(object sender, EventArgs e)
        {
            export_dgw_excel_1(this.dataGridView1);

            this.backgroundWorker3.CancelAsync();
            button1.Enabled = true;
            timer4.Enabled = false;
            timer5.Enabled = false;
            timer6.Enabled = false;
            timer7.Enabled = false;
            timer8.Enabled = false;
            timer9.Enabled = false;
            timer11.Enabled = false;
            timer12.Enabled = false;
            timer13.Enabled = false;
            timer14.Enabled = false;
            timer15.Enabled = false;
            timer16.Enabled = false;
            timer17.Enabled = false;
            Environment.Exit(0);
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            label17.Text = DateTime.Now.ToLongDateString();
            label3.Text = DateTime.Now.ToLongTimeString();
        }

        /* //////////////////////////////////////////////Picture Rotating START//////////////////////////////////////////////////// */

        private void timer2_Tick(object sender, EventArgs e)
        {
            string a = Convert.ToString(words[18], CultureInfo.InvariantCulture);
            NumberFormatInfo provider = new NumberFormatInfo();
            provider.NumberDecimalSeparator = ".";
            double anglee = double.Parse(a);
     
            Bitmap imagee = image_1;
            RotateBilinear ro = new RotateBilinear(anglee, true);
            Bitmap image22 = ro.Apply(imagee);
            pictureBox5.Image = image22;
        }
        /* //////////////////////////////////////////////Picture Rotating End//////////////////////////////////////////////////// */

        private void timer3_Tick(object sender, EventArgs e)
        {
            backgroundWorker2.RunWorkerAsync();
        }

        /* //////////////////////////////////////////////Chart Timer//////////////////////////////////////////////////// */
        private void timer4_Tick(object sender, EventArgs e)
        {
            chart1.ChartAreas[0].AxisX.Minimum = minm;  // basınç 1
            chart1.ChartAreas[0].AxisX.Maximum = maksm;
            chart1.ChartAreas[0].AxisY.Minimum = 0;
            chart1.ChartAreas[0].AxisY.Maximum = 100;
            chart1.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s = Convert.ToString(dataGridView1.Rows[i].Cells[3].Value);

            this.chart1.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s, CultureInfo.InvariantCulture));

            maksm++;
            minm++;
        }
        private void timer5_Tick(object sender, EventArgs e)
        { 
            chart5.ChartAreas[0].AxisX.Minimum = minm;  // basınç 2
            chart5.ChartAreas[0].AxisX.Maximum = maksm;
            chart5.ChartAreas[0].AxisY.Minimum = 0;
            chart5.ChartAreas[0].AxisY.Maximum = 100;
            chart5.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s2 = Convert.ToString(dataGridView1.Rows[i].Cells[4].Value);

            this.chart5.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s2, CultureInfo.InvariantCulture));
        }

        private void timer6_Tick(object sender, EventArgs e)
        {
            chart3.ChartAreas[0].AxisX.Minimum = minm;   // sıcaklık
            chart3.ChartAreas[0].AxisX.Maximum = maksm;
            chart3.ChartAreas[0].AxisY.Minimum = 0;
            chart3.ChartAreas[0].AxisY.Maximum = 100;
            chart3.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s3 = Convert.ToString(dataGridView1.Rows[i].Cells[9].Value);

            this.chart3.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s3, CultureInfo.InvariantCulture));
        }

        private void timer7_Tick(object sender, EventArgs e)
        {
            chart2.ChartAreas[0].AxisX.Minimum = minm;  // yükseklik 1
            chart2.ChartAreas[0].AxisX.Maximum = maksm;
            chart2.ChartAreas[0].AxisY.Minimum = 0;
            chart2.ChartAreas[0].AxisY.Maximum = 100;
            chart2.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s2 = Convert.ToString(dataGridView1.Rows[i].Cells[5].Value);

            this.chart2.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s2, CultureInfo.InvariantCulture));
        }
        private void timer12_Tick(object sender, EventArgs e)
        {
            chart6.ChartAreas[0].AxisX.Minimum = minm;  // yükseklik 2
            chart6.ChartAreas[0].AxisX.Maximum = maksm;
            chart6.ChartAreas[0].AxisY.Minimum = 0;
            chart6.ChartAreas[0].AxisY.Maximum = 100;
            chart6.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s2 = Convert.ToString(dataGridView1.Rows[i].Cells[6].Value);

            this.chart6.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s2, CultureInfo.InvariantCulture));
        }
        private void timer13_Tick(object sender, EventArgs e)
        {
            chart4.ChartAreas[0].AxisX.Minimum = minm;  // pil gerilimi
            chart4.ChartAreas[0].AxisX.Maximum = maksm;
            chart4.ChartAreas[0].AxisY.Minimum = 0;
            chart4.ChartAreas[0].AxisY.Maximum = 100;
            chart4.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s2 = Convert.ToString(dataGridView1.Rows[i].Cells[10].Value);

            this.chart4.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s2, CultureInfo.InvariantCulture));
        }
        private void timer15_Tick(object sender, EventArgs e)
        {
            chart8.ChartAreas[0].AxisX.Minimum = minm;  //iniş hızı
            chart8.ChartAreas[0].AxisX.Maximum = maksm;
            chart8.ChartAreas[0].AxisY.Minimum = 0;
            chart8.ChartAreas[0].AxisY.Maximum = 100;
            chart8.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s2 = Convert.ToString(dataGridView1.Rows[i].Cells[8].Value);

            this.chart8.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s2, CultureInfo.InvariantCulture));
        }
        private void timer16_Tick(object sender, EventArgs e)
        {
            chart9.ChartAreas[0].AxisX.Minimum = minm;  //Dönüş sayısı
            chart9.ChartAreas[0].AxisX.Maximum = maksm;
            chart9.ChartAreas[0].AxisY.Minimum = 0;
            chart9.ChartAreas[0].AxisY.Maximum = 100;
            chart9.ChartAreas[0].AxisX.ScaleView.Zoom(minm, maksm);
            string s2 = Convert.ToString(dataGridView1.Rows[i].Cells[21].Value);

            this.chart9.Series[0].Points.AddXY((minm + maksm) / 2, (float)double.Parse(s2, CultureInfo.InvariantCulture));
        }

        int progress_value=0;
        private void timer8_Tick(object sender, EventArgs e)
        {
            progress_value = (Convert.ToInt32(dataGridView1.Rows[i].Cells[17].Value));
            for(int k=0;k<5;k++)
            checkedListBox1.SetItemChecked(k, false);

            if (progress_value == 1)
            {
                progressBar1.Value = 20;
                checkedListBox1.SetItemChecked(0, true);
            }
               
            else if (progress_value == 2)
            {
                progressBar1.Value = 40;
                checkedListBox1.SetItemChecked(1, true);
            }
               
            else if (progress_value == 3)
            {
                progressBar1.Value = 60;
                checkedListBox1.SetItemChecked(2, true);
            }
                
            else if (progress_value == 4)
            {
                progressBar1.Value = 80;
                checkedListBox1.SetItemChecked(3, true);
            }
                
            else if (progress_value == 5)
            {
                progressBar1.Value = 100;
                checkedListBox1.SetItemChecked(4, true);
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            backgroundWorker3.RunWorkerAsync();
            button1.Enabled = false;
        }
        
        void recv(IAsyncResult res)
        {
            IPEndPoint RemoteIP = new IPEndPoint(IPAddress.Any,52000);
            byte[] received = Client.EndReceive(res,ref RemoteIP);
            mesaj = Encoding.UTF8.GetString(received);

            this.Invoke(new MethodInvoker(delegate
            {
                addData();
            }));
           
            Client.BeginReceive(new AsyncCallback(recv), null);
        }

        private void backgroundWorker3_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            Boolean flag = true;
            try
            {
                Client.BeginReceive(new AsyncCallback(recv), null);
            }
            catch(Exception ex)
            {
                mesaj = ex.Message.ToString();
            }

            byte[] msg = System.Text.Encoding.ASCII.GetBytes("1");
            while (true)
            {
                flag = true;
                while (flag)
                {
                    try
                    {
                        if (videoAaktarım == true)
                        {
                            msg = System.Text.Encoding.ASCII.GetBytes("2");
                        }
                        else if (manueltahrik == true)
                        {
                            msg = System.Text.Encoding.ASCII.GetBytes("3");
                        }
                        else if (manuelayırma == true)
                        {
                            msg = System.Text.Encoding.ASCII.GetBytes("4");
                        }
                        else
                        {
                            msg = System.Text.Encoding.ASCII.GetBytes("1");
                        }
                       
                        this.Invoke(new EventHandler(displayData_event));
                        videoAaktarım = false;
                        manueltahrik = false;
                        manuelayırma = false;
                    }
                    catch
                    {
                        /*dinle.Stop();
                        dinleyiciSoket.Close();*/
                        MessageBox.Show("Bağlantı koptu tekrar deneniyor");
                        //System.Threading.Thread.Sleep(500);
                    }
                }
            }
        }
        private void button2_Click(object sender, EventArgs e)
        {
            backgroundWorker3.CancelAsync();

            button1.Enabled = true;
            timer4.Enabled = false;
            timer5.Enabled = false;
            timer6.Enabled = false;
            timer7.Enabled = false;
            timer8.Enabled = false;
            timer9.Enabled = false;
            timer11.Enabled = false;
            timer12.Enabled = false;
            timer13.Enabled = false;
            timer14.Enabled = false;
            timer15.Enabled = false;
            timer16.Enabled = false;
            timer17.Enabled = false;

        }
        /* //////////////////////////////////////////////Chart Timer End//////////////////////////////////////////////////// */
       
        private void button4_Click(object sender, EventArgs e)
        {
            manuelayırma = true;
        }
        private void button5_Click(object sender, EventArgs e)
        {
            manueltahrik = true;
            MessageBox.Show("İşlem Gerçekleştirildi");
        }

        private void timer11_Tick(object sender, EventArgs e)
        {
            string a = Convert.ToString(dataGridView1.Rows[i].Cells[14].Value);
            string b = Convert.ToString(dataGridView1.Rows[i].Cells[15].Value);
            NumberFormatInfo provider = new NumberFormatInfo();
            provider.NumberDecimalSeparator = ".";

            latitude = double.Parse(a, provider);
            longitude = double.Parse(b, provider);
            this.gMapControl2.Position = new GMap.NET.PointLatLng(latitude, longitude);
        }
        struct FtpSetting
        {
            public string Server { get; set;}
            public string Username { get; set; }
            public string Password { get; set; }
            public string FileName { get; set; }
            public string FullName { get; set; }
        }
        FtpSetting _inputParameter;
        private void button6_Click(object sender, EventArgs e)
        {
            backgroundWorker6.RunWorkerAsync(_inputParameter);
            using(OpenFileDialog ofd = new OpenFileDialog())
            {
                if(ofd.ShowDialog() == DialogResult.OK)
                {
                    FileInfo fi = new FileInfo(ofd.FileName);
                    _inputParameter.Username = "android";
                    _inputParameter.Password = "android";
                    _inputParameter.Server = "ftp://192.168.1.47";
                    _inputParameter.FileName = fi.Name;
                    _inputParameter.FullName = fi.FullName;
                }
            }
        }
        private void backgroundWorker6_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            string fileName = ((FtpSetting)e.Argument).FileName;
            string fullname = ((FtpSetting)e.Argument).FullName;
            string userName = ((FtpSetting)e.Argument).Username;
            string password = ((FtpSetting)e.Argument).Password;
            string server = ((FtpSetting)e.Argument).Server;
            FtpWebRequest request = (FtpWebRequest)WebRequest.Create(new Uri(String.Format("{0}/{1}", server, fileName)));
            request.Method = WebRequestMethods.Ftp.UploadFile;
            request.Credentials = new NetworkCredential(userName, password);
            Stream ftpStream = request.GetRequestStream();
            FileStream fs = File.OpenRead(fullname);
            byte[] buffer = new byte[1024];
            double total = (double)fs.Length;
            int byteRead = 0;
            double read = 0;
            do
            {
                if (!backgroundWorker6.CancellationPending)
                {
                    byteRead = fs.Read(buffer, 0, 1024);
                    ftpStream.Write(buffer, 0, byteRead);
                    read += (double)byteRead;
                    double percentage = read / total * 100;
                    backgroundWorker6.ReportProgress((int)percentage);
                }
            }
            while (byteRead != 0);
            fs.Close();
            ftpStream.Close();
        }
        private void backgroundWorker6_ProgressChanged(object sender, System.ComponentModel.ProgressChangedEventArgs e)
        {
            lblstatus.Text = string.Format("Uploaded {0} %", e.ProgressPercentage);
            progressBar2.Value = e.ProgressPercentage;
            progressBar2.Update();
        }
        private void backgroundWorker6_RunWorkerCompleted(object sender, System.ComponentModel.RunWorkerCompletedEventArgs e)
        {
            lblstatus.Text = "%100 ! İletim Başarılı ";
        }

        private void timer17_Tick(object sender, EventArgs e)
        {
            label24.Text = timer17label;
        }

        private void timer10_Tick(object sender, EventArgs e)
        {
           
        }
        private void timer9_Tick(object sender, EventArgs e)
        {
            string a = Convert.ToString(dataGridView1.Rows[i].Cells[11].Value);
            string b = Convert.ToString(dataGridView1.Rows[i].Cells[12].Value);
            NumberFormatInfo provider = new NumberFormatInfo();
            provider.NumberDecimalSeparator = ".";

            latitude = double.Parse(a, provider);
            longitude = double.Parse(b, provider);
            this.gMapControl1.Position = new GMap.NET.PointLatLng(latitude, longitude);
        }
    }
}