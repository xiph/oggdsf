using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using illiminable.libDSPlayDotNET;
namespace testVideoImageGrab
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmTestGrab : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button cmdPlay;
		private System.Windows.Forms.PictureBox picGrabbed;
		private System.Windows.Forms.Button cmdGrabImage;


		private DSPlay mPlayer;
		private System.Windows.Forms.Panel pnlVideoWindow;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public frmTestGrab()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.pnlVideoWindow = new System.Windows.Forms.Panel();
			this.cmdPlay = new System.Windows.Forms.Button();
			this.picGrabbed = new System.Windows.Forms.PictureBox();
			this.cmdGrabImage = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// pnlVideoWindow
			// 
			this.pnlVideoWindow.Location = new System.Drawing.Point(32, 48);
			this.pnlVideoWindow.Name = "pnlVideoWindow";
			this.pnlVideoWindow.Size = new System.Drawing.Size(312, 280);
			this.pnlVideoWindow.TabIndex = 0;
			// 
			// cmdPlay
			// 
			this.cmdPlay.Location = new System.Drawing.Point(216, 344);
			this.cmdPlay.Name = "cmdPlay";
			this.cmdPlay.Size = new System.Drawing.Size(112, 32);
			this.cmdPlay.TabIndex = 1;
			this.cmdPlay.Text = "Play";
			this.cmdPlay.Click += new System.EventHandler(this.cmdPlay_Click);
			// 
			// picGrabbed
			// 
			this.picGrabbed.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.picGrabbed.Location = new System.Drawing.Point(504, 48);
			this.picGrabbed.Name = "picGrabbed";
			this.picGrabbed.Size = new System.Drawing.Size(328, 288);
			this.picGrabbed.TabIndex = 2;
			this.picGrabbed.TabStop = false;
			// 
			// cmdGrabImage
			// 
			this.cmdGrabImage.Location = new System.Drawing.Point(624, 360);
			this.cmdGrabImage.Name = "cmdGrabImage";
			this.cmdGrabImage.Size = new System.Drawing.Size(136, 32);
			this.cmdGrabImage.TabIndex = 3;
			this.cmdGrabImage.Text = "Grab Image";
			this.cmdGrabImage.Click += new System.EventHandler(this.cmdGrabImage_Click);
			// 
			// frmTestGrab
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(872, 422);
			this.Controls.Add(this.cmdGrabImage);
			this.Controls.Add(this.picGrabbed);
			this.Controls.Add(this.cmdPlay);
			this.Controls.Add(this.pnlVideoWindow);
			this.Name = "frmTestGrab";
			this.Text = "Test Grabber";
			this.Load += new System.EventHandler(this.frmTestGrab_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmTestGrab());
		}

		private void frmTestGrab_Load(object sender, System.EventArgs e)
		{
			mPlayer = new DSPlay(pnlVideoWindow.Handle, pnlVideoWindow.ClientRectangle.Left, pnlVideoWindow.ClientRectangle.Top, pnlVideoWindow.ClientRectangle.Width, pnlVideoWindow.ClientRectangle.Height);
			mPlayer.loadFile("G:\\320x240.ogg");
		}

		private void cmdPlay_Click(object sender, System.EventArgs e)
		{
			mPlayer.play();
		}

		private void cmdGrabImage_Click(object sender, System.EventArgs e)
		{
			System.Drawing.Bitmap locBM = mPlayer.GetImage();
			picGrabbed.Image = locBM;

		}


	
	}
}
