using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using illiminable.libDSPlayDotNET;
using illiminable.libCMMLTagsDotNET;


namespace DNPlay
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmDNPlay : System.Windows.Forms.Form, IDNMediaEvent, IDNCMMLCallbacks
	{
		private System.Windows.Forms.MainMenu mainMenu1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem2;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MenuItem menuItem4;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem menuItem6;
		private System.Windows.Forms.MenuItem menuItem7;
		private System.Windows.Forms.Button cmdPlay;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.Label lblFileLocation;
		private System.Windows.Forms.OpenFileDialog dlgOpenFile;
		private System.Windows.Forms.Button cmdStop;
		private System.Windows.Forms.Button cmdPause;
		private System.Windows.Forms.Label lblDuration;
		private System.Windows.Forms.Timer tmrUpdateDuration;
		private System.Windows.Forms.Label lblProgressBkgd;
		private System.Windows.Forms.Label lblProgressFgnd;

		//My Variables...
		protected DSPlay mPlayer;
		protected Int64 mFileDuration;
		protected Int64 mLastSync;
		private System.Windows.Forms.Timer tmrEventCheck;
		protected Int64 mNumTicks;
		protected ClipTag mCurrentClip;
		private System.Windows.Forms.Label lblClipDesc;
		private System.Windows.Forms.Button cmdFollowLink;
		private System.Windows.Forms.Label lblAnchorLink;
		private System.Windows.Forms.Label lblTitle;
		private System.Windows.Forms.Label label1;
		protected HeadTag mHeadTag;
		private Int64 evCount;
		private Uri mBaseURI; 
		private String mFileName;

		enum eEventCodes 
		{
			EC_COMPLETE = 1
		};
		//

		//My member functions
		protected void setDurationText(Int64 inDuration) 
		{
			lblDuration.Text = "";
			Int64 locSeconds = inDuration / 10000000;
			Int64 locMinutes = locSeconds / 60;
			Int64 locHours = locMinutes / 60;
			locMinutes = locMinutes % 60;
			locSeconds = locSeconds % 60;
			if (locHours != 0) 
			{
				lblDuration.Text = locHours.ToString() + ":";
				if (locMinutes < 10) 
				{
					lblDuration.Text += "0";
				}
			} 
			
			lblDuration.Text += locMinutes.ToString() + ":";
			
			if (locSeconds < 10) 
			{
				lblDuration.Text += "0";
			}
			lblDuration.Text += locSeconds.ToString();
		}

		protected void updateProgressBar() 
		{
			double locProgRatio = 0;
			Int32 locProgWidth = 0;
			if (mFileDuration > 0) 
			{
				try 
				{
					locProgRatio =  Convert.ToDouble(mLastSync + (mNumTicks * 10000000)) / mFileDuration;
					locProgWidth = Convert.ToInt32(locProgRatio * lblProgressBkgd.Width);
				} 
				catch (System.OverflowException)
				{
					//MessageBox.Show(mFileDuration.ToString());
					
				    locProgWidth = 0;
				}
			}
			
			lblProgressFgnd.Width = locProgWidth;
		}
		public bool eventNotification(Int32 inEventCode, Int32 inParam1, Int32 inParam2) 
		{
			evCount++;
			label1.Text = evCount.ToString();
			if (inEventCode == (long)eEventCodes.EC_COMPLETE) 
			{
				tmrUpdateDuration.Enabled = false;
				mPlayer.stop();
			}
			return true;
		}

		//Implementing IDNCMMLCallbacks
		public bool clipCallback(ClipTag inClipTag) 
		{
			mCurrentClip = inClipTag;
			lblClipDesc.Text = inClipTag.desc().text();
			lblAnchorLink.Text = inClipTag.anchor().href();
			return true;
		}
		public bool headCallback(HeadTag inHeadTag) 
		{
			MessageBox.Show("Head callback");
			mHeadTag = inHeadTag;
			if (mHeadTag != null) 
			{
				MessageBox.Show("Head tag not null");
			}
			else 
			{
				MessageBox.Show("Head tag is null");
			}
			lblTitle.Text = mHeadTag.title().text();
			MessageBox.Show(mHeadTag.title().text());
			Uri locBaseURI = null;
			try 
			{
				if (mHeadTag.@base() != null) 
				{
					MessageBox.Show("Href = "+mHeadTag.@base().href());
					locBaseURI = new Uri(mHeadTag.@base().href());
				}
			} 
			catch(System.UriFormatException) 
			{
				locBaseURI = null;	
					
			}

			if (locBaseURI != null) 
			{
				mBaseURI = locBaseURI;
			}
		
			MessageBox.Show("Bug not here !!");
			return true;
		}
		//

		public frmDNPlay()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			mPlayer = new DSPlay();
			lblProgressFgnd.Width = 0;
			cmdStop.Enabled = false;
			cmdPlay.Enabled = false;
			cmdPause.Enabled = false;

			evCount = 0;
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
			this.components = new System.ComponentModel.Container();
			this.mainMenu1 = new System.Windows.Forms.MainMenu();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.menuItem3 = new System.Windows.Forms.MenuItem();
			this.menuItem4 = new System.Windows.Forms.MenuItem();
			this.menuItem5 = new System.Windows.Forms.MenuItem();
			this.menuItem6 = new System.Windows.Forms.MenuItem();
			this.menuItem2 = new System.Windows.Forms.MenuItem();
			this.menuItem7 = new System.Windows.Forms.MenuItem();
			this.cmdPlay = new System.Windows.Forms.Button();
			this.lblFileLocation = new System.Windows.Forms.Label();
			this.dlgOpenFile = new System.Windows.Forms.OpenFileDialog();
			this.cmdStop = new System.Windows.Forms.Button();
			this.cmdPause = new System.Windows.Forms.Button();
			this.lblDuration = new System.Windows.Forms.Label();
			this.tmrUpdateDuration = new System.Windows.Forms.Timer(this.components);
			this.lblProgressBkgd = new System.Windows.Forms.Label();
			this.lblProgressFgnd = new System.Windows.Forms.Label();
			this.tmrEventCheck = new System.Windows.Forms.Timer(this.components);
			this.lblClipDesc = new System.Windows.Forms.Label();
			this.cmdFollowLink = new System.Windows.Forms.Button();
			this.lblAnchorLink = new System.Windows.Forms.Label();
			this.lblTitle = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// mainMenu1
			// 
			this.mainMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem1,
																					  this.menuItem2});
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 0;
			this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem3,
																					  this.menuItem4,
																					  this.menuItem5,
																					  this.menuItem6});
			this.menuItem1.Text = "&File";
			// 
			// menuItem3
			// 
			this.menuItem3.Index = 0;
			this.menuItem3.Text = "&Open...";
			this.menuItem3.Click += new System.EventHandler(this.menuItem3_Click);
			// 
			// menuItem4
			// 
			this.menuItem4.Index = 1;
			this.menuItem4.Text = "Open &URL...";
			this.menuItem4.Click += new System.EventHandler(this.menuItem4_Click);
			// 
			// menuItem5
			// 
			this.menuItem5.Index = 2;
			this.menuItem5.Text = "-";
			// 
			// menuItem6
			// 
			this.menuItem6.Index = 3;
			this.menuItem6.Text = "E&xit";
			this.menuItem6.Click += new System.EventHandler(this.menuItem6_Click);
			// 
			// menuItem2
			// 
			this.menuItem2.Index = 1;
			this.menuItem2.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					  this.menuItem7});
			this.menuItem2.Text = "Help";
			// 
			// menuItem7
			// 
			this.menuItem7.Index = 0;
			this.menuItem7.Text = "&About";
			// 
			// cmdPlay
			// 
			this.cmdPlay.Location = new System.Drawing.Point(8, 80);
			this.cmdPlay.Name = "cmdPlay";
			this.cmdPlay.Size = new System.Drawing.Size(56, 24);
			this.cmdPlay.TabIndex = 0;
			this.cmdPlay.Text = "&Play";
			this.cmdPlay.Click += new System.EventHandler(this.cmdPlay_Click);
			// 
			// lblFileLocation
			// 
			this.lblFileLocation.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblFileLocation.Location = new System.Drawing.Point(8, 8);
			this.lblFileLocation.Name = "lblFileLocation";
			this.lblFileLocation.Size = new System.Drawing.Size(400, 16);
			this.lblFileLocation.TabIndex = 3;
			// 
			// dlgOpenFile
			// 
			this.dlgOpenFile.Title = "Select a file to play...";
			// 
			// cmdStop
			// 
			this.cmdStop.Location = new System.Drawing.Point(72, 80);
			this.cmdStop.Name = "cmdStop";
			this.cmdStop.Size = new System.Drawing.Size(56, 24);
			this.cmdStop.TabIndex = 5;
			this.cmdStop.Text = "&Stop";
			this.cmdStop.Click += new System.EventHandler(this.cmdStop_Click);
			// 
			// cmdPause
			// 
			this.cmdPause.Location = new System.Drawing.Point(136, 80);
			this.cmdPause.Name = "cmdPause";
			this.cmdPause.Size = new System.Drawing.Size(56, 24);
			this.cmdPause.TabIndex = 6;
			this.cmdPause.Text = "Pa&use";
			this.cmdPause.Click += new System.EventHandler(this.cmdPause_Click);
			// 
			// lblDuration
			// 
			this.lblDuration.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblDuration.Font = new System.Drawing.Font("Comic Sans MS", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblDuration.Location = new System.Drawing.Point(240, 80);
			this.lblDuration.Name = "lblDuration";
			this.lblDuration.Size = new System.Drawing.Size(168, 24);
			this.lblDuration.TabIndex = 7;
			this.lblDuration.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tmrUpdateDuration
			// 
			this.tmrUpdateDuration.Interval = 1000;
			this.tmrUpdateDuration.Tick += new System.EventHandler(this.tmrUpdateDuration_Tick);
			// 
			// lblProgressBkgd
			// 
			this.lblProgressBkgd.BackColor = System.Drawing.SystemColors.ControlDark;
			this.lblProgressBkgd.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblProgressBkgd.Location = new System.Drawing.Point(8, 56);
			this.lblProgressBkgd.Name = "lblProgressBkgd";
			this.lblProgressBkgd.Size = new System.Drawing.Size(400, 16);
			this.lblProgressBkgd.TabIndex = 8;
			this.lblProgressBkgd.Click += new System.EventHandler(this.lblProgressBkgd_Click);
			this.lblProgressBkgd.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lblProgressBkgd_MouseDown);
			// 
			// lblProgressFgnd
			// 
			this.lblProgressFgnd.BackColor = System.Drawing.Color.SeaGreen;
			this.lblProgressFgnd.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblProgressFgnd.Location = new System.Drawing.Point(8, 56);
			this.lblProgressFgnd.Name = "lblProgressFgnd";
			this.lblProgressFgnd.Size = new System.Drawing.Size(232, 16);
			this.lblProgressFgnd.TabIndex = 9;
			this.lblProgressFgnd.MouseDown += new System.Windows.Forms.MouseEventHandler(this.lblProgressBkgd_MouseDown);
			// 
			// tmrEventCheck
			// 
			this.tmrEventCheck.Interval = 250;
			this.tmrEventCheck.Tick += new System.EventHandler(this.tmrEventCheck_Tick);
			// 
			// lblClipDesc
			// 
			this.lblClipDesc.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblClipDesc.Location = new System.Drawing.Point(8, 120);
			this.lblClipDesc.Name = "lblClipDesc";
			this.lblClipDesc.Size = new System.Drawing.Size(400, 104);
			this.lblClipDesc.TabIndex = 10;
			// 
			// cmdFollowLink
			// 
			this.cmdFollowLink.Location = new System.Drawing.Point(312, 232);
			this.cmdFollowLink.Name = "cmdFollowLink";
			this.cmdFollowLink.Size = new System.Drawing.Size(96, 24);
			this.cmdFollowLink.TabIndex = 11;
			this.cmdFollowLink.Text = "Follow Link";
			this.cmdFollowLink.Click += new System.EventHandler(this.cmdFollowLink_Click);
			// 
			// lblAnchorLink
			// 
			this.lblAnchorLink.Location = new System.Drawing.Point(56, 232);
			this.lblAnchorLink.Name = "lblAnchorLink";
			this.lblAnchorLink.Size = new System.Drawing.Size(248, 24);
			this.lblAnchorLink.TabIndex = 12;
			// 
			// lblTitle
			// 
			this.lblTitle.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblTitle.Location = new System.Drawing.Point(8, 32);
			this.lblTitle.Name = "lblTitle";
			this.lblTitle.Size = new System.Drawing.Size(400, 16);
			this.lblTitle.TabIndex = 13;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 264);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(112, 32);
			this.label1.TabIndex = 14;
			this.label1.Text = "label1";
			// 
			// frmDNPlay
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(416, 295);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.lblTitle);
			this.Controls.Add(this.lblAnchorLink);
			this.Controls.Add(this.cmdFollowLink);
			this.Controls.Add(this.lblClipDesc);
			this.Controls.Add(this.lblProgressFgnd);
			this.Controls.Add(this.lblProgressBkgd);
			this.Controls.Add(this.lblDuration);
			this.Controls.Add(this.cmdPause);
			this.Controls.Add(this.cmdStop);
			this.Controls.Add(this.lblFileLocation);
			this.Controls.Add(this.cmdPlay);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximizeBox = false;
			this.Menu = this.mainMenu1;
			this.Name = "frmDNPlay";
			this.Text = "DNPlay";
			this.Load += new System.EventHandler(this.frmDNPlay_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmDNPlay());
		}

		
		private void OpenFile() 
		{
			//File->Open
			dlgOpenFile.CheckFileExists = true;
			DialogResult locResult = dlgOpenFile.ShowDialog();
			if (locResult == DialogResult.OK) 
			{
				LoadFile(dlgOpenFile.FileName);
//				tmrUpdateDuration.Enabled = false;
//				lblFileLocation.Text = dlgOpenFile.FileName;
//				bool locRes = mPlayer.loadFile(dlgOpenFile.FileName);
//				//Error check
//				mFileDuration = mPlayer.fileDuration();
//				setDurationText(mFileDuration);
//				//lblDuration.Text = mFileDuration.ToString();
//
//				mNumTicks = 0;
//				mLastSync = 0;
//				updateProgressBar();
//
//				mPlayer.setMediaEventCallback(this);
//				mPlayer.setCMMLCallbacks(this);
//				cmdPlay.Enabled = true;
			}

		}

		private void LoadFile(String inFileName) 
		{
			tmrUpdateDuration.Enabled = false;

			bool locRes = mPlayer.loadFile(inFileName);

			if (locRes) 
			{
				mFileName = inFileName;
				lblFileLocation.Text = inFileName;

				
				//Set the base URI from the current file.
				setBaseURIFromFullPath(inFileName);
					
				
				//Error check
				mFileDuration = mPlayer.fileDuration();
				setDurationText(mFileDuration);
				//lblDuration.Text = mFileDuration.ToString();

				mNumTicks = 0;
				mLastSync = 0;
				updateProgressBar();

				mPlayer.setMediaEventCallback(this);
				mPlayer.setCMMLCallbacks(this);
				cmdPlay.Enabled = true;
			} 
			else 
			{
				MessageBox.Show("File type is unrecognised, or media file does not exist", "Media Open Failed.", MessageBoxButtons.OK, MessageBoxIcon.Stop);
			
			}
		}
		
		private void menuItem3_Click(object sender, System.EventArgs e)
		{
			OpenFile();
			
		}

		private void StartPlayback() 
		{
			mPlayer.play();
			tmrUpdateDuration.Enabled = true;
			tmrEventCheck.Enabled = true;
			cmdPlay.Enabled = false;
			cmdPause.Enabled = true;
			cmdStop.Enabled = true;
		}
		private void cmdPlay_Click(object sender, System.EventArgs e)
		{
			StartPlayback();
		}

		private void StopPlayback() 
		{
			tmrUpdateDuration.Enabled = false;
			tmrEventCheck.Enabled = false;
			mPlayer.stop();
			//Need to seek to start here !
			mPlayer.seek(0);
			mNumTicks = 0;
			mLastSync = 0;
			updateProgressBar();
			cmdPause.Enabled = true;
			cmdStop.Enabled = false;
			cmdPlay.Enabled = true;
		}
		private void cmdStop_Click(object sender, System.EventArgs e)
		{
			StopPlayback();
		}

		private void PausePlayback() 
		{
			tmrEventCheck.Enabled = false;
			tmrUpdateDuration.Enabled = false;
			mPlayer.pause();
			cmdPause.Enabled = false;
			cmdPlay.Enabled = true;
			cmdStop.Enabled = true;
		}
		private void cmdPause_Click(object sender, System.EventArgs e)
		{
			PausePlayback();
		}

		private void frmDNPlay_Load(object sender, System.EventArgs e)
		{
		
		}

		private void tmrUpdateDuration_Tick(object sender, System.EventArgs e)
		{
			mNumTicks++;
			updateProgressBar();
		}

		private void menuItem6_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void tmrEventCheck_Tick(object sender, System.EventArgs e)
		{
			mPlayer.checkEvents();
		}

		private void lblProgressBkgd_Click(object sender, System.EventArgs e)
		{
		
		}

		private void lblProgressBkgd_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e) 
		{
			//This is the event for both the foreground and the background label.
			

			if (e.Button == MouseButtons.Left)
			{
				double locSeekRatio = ((double)e.X) / (double)lblProgressBkgd.Width;
				Int64 locSeekPoint = (Int64)(mFileDuration * locSeekRatio);
				locSeekPoint = mPlayer.seek(locSeekPoint);
				if (locSeekPoint != -1) 
				{
					mLastSync = locSeekPoint;
					mNumTicks = 0;
					updateProgressBar();
				}
				
			}
		}

		private bool setBaseURIFromFullPath(String inFullPath) 
		{
			MessageBox.Show(inFullPath);
			Uri locURI = null;
			Uri locBaseURI = null;
			try 
			{
				//Turn the full path into a URI
				locURI = new Uri(inFullPath);
			}
			catch (System.UriFormatException) 
			{
				//This is not a URI !
				locURI = null;
			}

			if (locURI != null) 
			{
				MessageBox.Show(locURI.ToString());
				MessageBox.Show(locURI.GetLeftPart(UriPartial.Authority));
				String locPartial = locURI.GetLeftPart(UriPartial.Authority);

				if (locPartial.Equals("")) 
				{

					//Must be a file with a : 'd path in it
					locPartial = locURI.GetLeftPart(UriPartial.Path);

					//Find out where the lat slash is
					int locDelimPos = locPartial.LastIndexOf("/");
					if (locDelimPos != -1) 
					{
						//Strip off the filename part at the end
						locPartial = locPartial.Substring(0, locDelimPos + 1);
						MessageBox.Show("Parital : " + locPartial);
					} 
					else 
					{
						locPartial = "";
					}
				}
				//Get the URI base which excludes the filename part.
				
				if (!locPartial.Equals("")) 
				{
					locBaseURI = new Uri(locPartial);
					MessageBox.Show(locBaseURI.ToString());
				}
				
			} 

			mBaseURI = locBaseURI;

			return (locBaseURI != null);


		}
		private void cmdFollowLink_Click(object sender, System.EventArgs e)
		{
			
			if (mBaseURI != null) 
			{
				//Try to do relative to the base URI
				Uri locURI = null;
				try 
				{
					MessageBox.Show("Base is "+mBaseURI.ToString());
					//try and make a URI using the base and the href from the clip (relative)
					locURI = new Uri(mBaseURI, mCurrentClip.anchor().href());	
					MessageBox.Show("New URI is "+locURI.ToString());
				} 
				catch(System.UriFormatException) 
				{
					try 
					{
						//If that failed, try to make one with just the clip tag (absolute)
						locURI = new Uri(mCurrentClip.anchor().href());
					}
					catch (System.UriFormatException) 
					{
						locURI = null;
						

					}
				}

				if (locURI == null) 
				{
					MessageBox.Show("The link is an invalid URI", "Invalid URI", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
				} 
				else
				{
					MessageBox.Show("Opening "+locURI.ToString());
					if (locURI.IsFile) 
					{
						//If it's a file change it to a local path.
						LoadFile(locURI.LocalPath);
						StartPlayback();

					} 
					else 
					{
						//Otherwise just load the URI
						LoadFile(locURI.ToString());
						StartPlayback();
					}
				}
			} 
			
			
//			tmrUpdateDuration.Enabled = false;
//			lblFileLocation.Text = locFilename;
//			bool locRes = mPlayer.loadFile("G:\\downloads\\firefox\\manufacturing_surveys.anx");
//			//Error check
//			mFileDuration = mPlayer.fileDuration();
//			setDurationText(mFileDuration);
//			//lblDuration.Text = mFileDuration.ToString();
//
//			mNumTicks = 0;
//			mLastSync = 0;
//			updateProgressBar();
//
//			mPlayer.setMediaEventCallback(this);
//			mPlayer.setCMMLCallbacks(this);
//			cmdPlay.Enabled = true;
//			StartPlayback();
		}

		private void menuItem4_Click(object sender, System.EventArgs e)
		{
			frmOpenURL locOpenDialog = new frmOpenURL();
			locOpenDialog.ShowDialog(this);
			if (locOpenDialog.wasOK) 
			{
				LoadFile(locOpenDialog.URLToOpen);
				
				
				
			}
		}

	}
}
