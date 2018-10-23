using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MyCamerN;
using IPCameraDll;
using System.IO;
using System.Timers;
namespace 视频录制与播放
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            ipc = new IpCameraHelper("10.96.34.171:80", "admin", "", new Action<ImageEventArgs>((a) => { bit = a.FrameReadyEventArgs.BitmapImage; }));
            ipc.StartProcessing();
            sto.IsEnabled = false;
            gam.IsEnabled = true;
            ti = new Timer(50);
            ti.Elapsed += new ElapsedEventHandler(tim1);
            ti2 = new Timer(50);
            ti2.Elapsed += new ElapsedEventHandler(tim2);
           // ti.BeginInit();
            
        }
        Timer ti;
        Timer ti2;
        IpCameraHelper ipc;
        BitmapImage bit;
        FileStream file;
        BinaryWriter bin;
        Stream st;
        BinaryReader bin2;
        
        private void tim1(object sender, ElapsedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                try
                {
                    st = bit.StreamSource;
                    st.Position = 0;
                    bin2 = new BinaryReader(st);
                    byte[] a = new byte[st.Length + 4];
                    byte[] b = bin2.ReadBytes((int)st.Length);
                    int a2 = b.Length;
                    a[0] = (byte)(a2 >> 24);
                    a[1] = (byte)(a2 >> 16);
                    a[2] = (byte)(a2 >> 8);
                    a[3] = (byte)(a2);
                    for (int x = 4; x < a.Length; x++)
                    {
                        a[x] = b[x - 4];
                    }
                    // bin2.Close();
                    //  st.Close();


                    bin.Write(a, 0, a.Length);
                    img.Source = bit;
                }
                catch { }
            });
        }
        private void tim2(object sender, ElapsedEventArgs e)
        {
            this.Dispatcher.Invoke(() =>
            {
                try
                {
                    int x;
                    byte[] a = bi.ReadBytes(4);

                    x = a[0];
                    x = x << 8;
                    x |= a[1];
                    x = x << 8;
                    x |= a[2];
                    x = x << 8;
                    x |= a[3];
                    byte[] b = bi.ReadBytes(x);

                    BitmapImage bitt = new BitmapImage();
                    bitt.BeginInit();
                    bitt.StreamSource = new MemoryStream(b);
                    bitt.EndInit();
                    img.Source = bitt;
                }
                catch (Exception es)
                {
                    fil.Position = 0;
                }

            });
        }
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            sto.IsEnabled = true;
            gam.IsEnabled = false;
            sta.IsEnabled = false;
            file = new FileStream("../demo.my", FileMode.Create, FileAccess.Write);
            bin = new BinaryWriter(file);
            ti.Start();

        }

        private void sto_Click(object sender, RoutedEventArgs e)
        {
            sto.IsEnabled = false;
            gam.IsEnabled = true;
            sta.IsEnabled = true;
            ti.Stop();
            bin.Close();
            file.Close();

            bin2.Close();
            st.Close();
        }
        FileStream fil;
        BinaryReader bi;
        
        private void gam_Click(object sender, RoutedEventArgs e)
        {
            fil = new FileStream("../demo.my", FileMode.Open, FileAccess.Read);
            fil.Position = 0;
            bi = new BinaryReader(fil);
            
            ti2.Start();
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            ipc.PanUp();
        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {
            ipc.PanDown();
        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            ipc.PanLeft();
        }

        private void Button_Click_5(object sender, RoutedEventArgs e)
        {
            ipc.PanRight();
        }
    }
}
