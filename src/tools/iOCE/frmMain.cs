using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace iOCE
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class frmMain : System.Windows.Forms.Form
	{
		private System.Windows.Forms.MainMenu mnuMain;
		private System.Windows.Forms.MenuItem mnuFile;
		private System.Windows.Forms.MenuItem mnuFileOpen;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem mnuFileExit;
		private System.Windows.Forms.Label lblVendor;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox txtVendorString;
		private System.Windows.Forms.ColumnHeader colKey;
		private System.Windows.Forms.ColumnHeader colValue;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.OpenFileDialog dlgOpenFile;
		private System.Windows.Forms.ListView lsvUserComments;
		private System.Windows.Forms.Button cmdAdd;
		private System.Windows.Forms.Button cmdRemove;
		private System.Windows.Forms.Button cmdEdit;
		private System.Windows.Forms.Button cmdApply;
		private System.Windows.Forms.Button cmdRevert;


		private illiminable.Ogg.libVorbisCommentDotNET.DNFileComments mFileComments;

		private void fillFields() 
		{
			txtVendorString.Text = mFileComments.getStreamComment(0).comments().vendorString();
			String[] locSubItems = null; 
			ListViewItem locItem = null;
			
			for (int i = 0; i < mFileComments.getStreamComment(0).comments().numUserComments(); i++) 
			{
				locSubItems = new String[2];
				
				locSubItems[0] = mFileComments.getStreamComment(0).comments().getUserComment(i).key();
				locSubItems[1] = mFileComments.getStreamComment(0).comments().getUserComment(i).value();
				locItem = new ListViewItem(locSubItems);
				lsvUserComments.Items.Add(locItem);
			}
			
			


		}
		public frmMain()
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
			this.mnuMain = new System.Windows.Forms.MainMenu();
			this.mnuFile = new System.Windows.Forms.MenuItem();
			this.mnuFileOpen = new System.Windows.Forms.MenuItem();
			this.menuItem1 = new System.Windows.Forms.MenuItem();
			this.mnuFileExit = new System.Windows.Forms.MenuItem();
			this.lblVendor = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.cmdEdit = new System.Windows.Forms.Button();
			this.cmdRemove = new System.Windows.Forms.Button();
			this.cmdAdd = new System.Windows.Forms.Button();
			this.lsvUserComments = new System.Windows.Forms.ListView();
			this.colKey = new System.Windows.Forms.ColumnHeader();
			this.colValue = new System.Windows.Forms.ColumnHeader();
			this.txtVendorString = new System.Windows.Forms.TextBox();
			this.dlgOpenFile = new System.Windows.Forms.OpenFileDialog();
			this.cmdApply = new System.Windows.Forms.Button();
			this.cmdRevert = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// mnuMain
			// 
			this.mnuMain.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuFile});
			// 
			// mnuFile
			// 
			this.mnuFile.Index = 0;
			this.mnuFile.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																					this.mnuFileOpen,
																					this.menuItem1,
																					this.mnuFileExit});
			this.mnuFile.Text = "&File";
			// 
			// mnuFileOpen
			// 
			this.mnuFileOpen.Checked = true;
			this.mnuFileOpen.Index = 0;
			this.mnuFileOpen.Text = "&Open";
			this.mnuFileOpen.Click += new System.EventHandler(this.mnuFileOpen_Click);
			// 
			// menuItem1
			// 
			this.menuItem1.Index = 1;
			this.menuItem1.Text = "-";
			// 
			// mnuFileExit
			// 
			this.mnuFileExit.Index = 2;
			this.mnuFileExit.Text = "E&xit";
			// 
			// lblVendor
			// 
			this.lblVendor.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lblVendor.Location = new System.Drawing.Point(16, 24);
			this.lblVendor.Name = "lblVendor";
			this.lblVendor.Size = new System.Drawing.Size(88, 16);
			this.lblVendor.TabIndex = 0;
			this.lblVendor.Text = "Vendor String";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.cmdEdit);
			this.groupBox1.Controls.Add(this.cmdRemove);
			this.groupBox1.Controls.Add(this.cmdAdd);
			this.groupBox1.Controls.Add(this.lsvUserComments);
			this.groupBox1.Controls.Add(this.txtVendorString);
			this.groupBox1.Controls.Add(this.lblVendor);
			this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.groupBox1.Location = new System.Drawing.Point(16, 48);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(632, 224);
			this.groupBox1.TabIndex = 1;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Vorbis Comments";
			// 
			// cmdEdit
			// 
			this.cmdEdit.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cmdEdit.Location = new System.Drawing.Point(440, 192);
			this.cmdEdit.Name = "cmdEdit";
			this.cmdEdit.Size = new System.Drawing.Size(80, 24);
			this.cmdEdit.TabIndex = 5;
			this.cmdEdit.Text = "&Edit...";
			this.cmdEdit.Click += new System.EventHandler(this.cmdEdit_Click);
			// 
			// cmdRemove
			// 
			this.cmdRemove.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cmdRemove.Location = new System.Drawing.Point(536, 192);
			this.cmdRemove.Name = "cmdRemove";
			this.cmdRemove.Size = new System.Drawing.Size(80, 24);
			this.cmdRemove.TabIndex = 4;
			this.cmdRemove.Text = "&Remove";
			// 
			// cmdAdd
			// 
			this.cmdAdd.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.cmdAdd.Location = new System.Drawing.Point(344, 192);
			this.cmdAdd.Name = "cmdAdd";
			this.cmdAdd.Size = new System.Drawing.Size(80, 24);
			this.cmdAdd.TabIndex = 3;
			this.cmdAdd.Text = "&Add...";
			this.cmdAdd.Click += new System.EventHandler(this.cmdAdd_Click);
			// 
			// lsvUserComments
			// 
			this.lsvUserComments.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																							  this.colKey,
																							  this.colValue});
			this.lsvUserComments.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.lsvUserComments.FullRowSelect = true;
			this.lsvUserComments.GridLines = true;
			this.lsvUserComments.Location = new System.Drawing.Point(16, 48);
			this.lsvUserComments.MultiSelect = false;
			this.lsvUserComments.Name = "lsvUserComments";
			this.lsvUserComments.Size = new System.Drawing.Size(600, 128);
			this.lsvUserComments.TabIndex = 2;
			this.lsvUserComments.View = System.Windows.Forms.View.Details;
			// 
			// colKey
			// 
			this.colKey.Text = "Key";
			this.colKey.Width = 162;
			// 
			// colValue
			// 
			this.colValue.Text = "Value";
			this.colValue.Width = 433;
			// 
			// txtVendorString
			// 
			this.txtVendorString.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.txtVendorString.Location = new System.Drawing.Point(104, 20);
			this.txtVendorString.Name = "txtVendorString";
			this.txtVendorString.Size = new System.Drawing.Size(512, 20);
			this.txtVendorString.TabIndex = 1;
			this.txtVendorString.Text = "";
			// 
			// cmdApply
			// 
			this.cmdApply.Location = new System.Drawing.Point(456, 280);
			this.cmdApply.Name = "cmdApply";
			this.cmdApply.Size = new System.Drawing.Size(80, 24);
			this.cmdApply.TabIndex = 2;
			this.cmdApply.Text = "A&pply";
			// 
			// cmdRevert
			// 
			this.cmdRevert.Location = new System.Drawing.Point(552, 280);
			this.cmdRevert.Name = "cmdRevert";
			this.cmdRevert.Size = new System.Drawing.Size(80, 24);
			this.cmdRevert.TabIndex = 3;
			this.cmdRevert.Text = "Re&vert";
			// 
			// frmMain
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(664, 313);
			this.Controls.Add(this.cmdRevert);
			this.Controls.Add(this.cmdApply);
			this.Controls.Add(this.groupBox1);
			this.Menu = this.mnuMain;
			this.Name = "frmMain";
			this.Text = "illiminable Ogg Comment Editor";
			this.Load += new System.EventHandler(this.frmMain_Load);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new frmMain());
		}

		private void frmMain_Load(object sender, System.EventArgs e)
		{
			mFileComments = new illiminable.Ogg.libVorbisCommentDotNET.DNFileComments();
		}

		private void mnuFileOpen_Click(object sender, System.EventArgs e)
		{
			dlgOpenFile.ShowDialog();

			mFileComments.loadFile(dlgOpenFile.FileName);

			fillFields();
		}

		

		private void cmdAdd_Click(object sender, System.EventArgs e)
		{
			
			ListViewItem locItem = null;

			frmAddComment locAddForm = new frmAddComment();
			locAddForm.ShowDialog();
			if (locAddForm.wasOK == true) 
			{
				String[] locSubItems = new String[2];
				locSubItems[0] = locAddForm.Key;
				locSubItems[1] = locAddForm.Value;
				locItem = new ListViewItem(locSubItems);
				lsvUserComments.Items.Add(locItem);
			}
			
		}

		private void cmdEdit_Click(object sender, System.EventArgs e)
		{
			ListView.SelectedListViewItemCollection locSelected = lsvUserComments.SelectedItems;
			
			
			
			if (locSelected.Count == 1) 
			{
				ListViewItem locItem = locSelected[0];
				ListViewItem.ListViewSubItemCollection locSubItems = locItem.SubItems;
				
				frmAddComment locAddForm = new frmAddComment();
				locAddForm.Text = "Edit Comment...";
				locAddForm.Key = locSubItems[0].Text;
				locAddForm.Value = locSubItems[1].Text;
			
				
				locAddForm.ShowDialog();
				if (locAddForm.wasOK == true) 
				{
					String[] locSubItemStr = new String[2];
					locSubItemStr[0] = locAddForm.Key;
					locSubItemStr[1] = locAddForm.Value;
					lsvUserComments.Items.Remove(locItem);
					locItem = new ListViewItem(locSubItemStr);
					
					lsvUserComments.Items.Add(locItem);
				}
			}
		}
	}
}
