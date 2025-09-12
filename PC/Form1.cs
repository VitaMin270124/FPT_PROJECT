using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CanFirmwareUploader
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void ButtonSelect_Click(object sender, EventArgs e)
        {
            // Tạo hộp thoại chọn file
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                // Chỉ cho phép chọn file có đuôi .bin
                ofd.Filter = "BIN files (*.bin)|*.bin";

                // Nếu người dùng chọn file và bấm OK
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    // Gán đường dẫn file vào TextBox
                    txtFilePath.Text = ofd.FileName;
                }
            }
        }

        private async void ButtonUpload_Click(object sender, EventArgs e)
        {
            // 1. Kiểm tra đã chọn file chưa
            if (string.IsNullOrWhiteSpace(txtFilePath.Text))
            {
                MessageBox.Show("Vui lòng chọn file .bin trước khi tải lên!", "Lỗi", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            // 2. Địa chỉ IP của ESP32 (bạn thay đúng IP của mình)
            string url = "http://192.168.4.1:8080/upload"; // Ví dụ IP của ESP32 ở chế độ AP

            try
            {
                lblStatus.Text = "Đang tải lên...";
                progressBar1.Style = ProgressBarStyle.Marquee; // Chạy dạng "loading"

                using (HttpClient client = new HttpClient())
                using (var fs = new FileStream(txtFilePath.Text, FileMode.Open, FileAccess.Read))
                using (var content = new MultipartFormDataContent())
                {
                    var progressContent = new ProgressStreamContent(fs, 4096, (sent, total) =>
                    {
                        int percent = (int)((sent * 100) / total);
                        progressBar1.Invoke((Action)(() => progressBar1.Value = percent));
                        lblStatus.Invoke((Action)(() => lblStatus.Text = $"Đang tải lên: {percent}%"));
                    });

                    content.Add(progressContent, "firmware", Path.GetFileName(txtFilePath.Text));

                    HttpResponseMessage response = await client.PostAsync(url, content);

                    if (response.IsSuccessStatusCode)
                    {
                        lblStatus.Text = "Tải lên thành công!";
                        MessageBox.Show("Firmware đã được tải lên ESP32 thành công!", "Thành công", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        lblStatus.Text = "Tải lên thất bại!";
                        MessageBox.Show($"Lỗi: {response.StatusCode}", "Thất bại", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }

            }
            catch (Exception ex)
            {
                lblStatus.Text = "Lỗi khi tải lên!";
                MessageBox.Show($"Lỗi: {ex.Message}", "Lỗi", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                progressBar1.Style = ProgressBarStyle.Blocks; // Trả về chế độ bình thường
            }
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void lblStatus_Click(object sender, EventArgs e)
        {

        }
    }
    public class ProgressStreamContent : HttpContent
    {
        private readonly Stream _stream;
        private readonly int _bufferSize;
        private readonly Action<long, long> _progress; // bytesSent, totalBytes

        public ProgressStreamContent(Stream stream, int bufferSize, Action<long, long> progress)
        {
            _stream = stream;
            _bufferSize = bufferSize;
            _progress = progress;
        }

        protected override async Task SerializeToStreamAsync(Stream targetStream, TransportContext context)
        {
            var buffer = new byte[_bufferSize];
            long size = _stream.Length;
            long uploaded = 0;
            int read;

            while ((read = await _stream.ReadAsync(buffer, 0, buffer.Length)) > 0)
            {
                await targetStream.WriteAsync(buffer, 0, read);
                uploaded += read;
                _progress?.Invoke(uploaded, size);
            }
        }

        protected override bool TryComputeLength(out long length)
        {
            length = _stream.Length;
            return true;
        }
    }

}
