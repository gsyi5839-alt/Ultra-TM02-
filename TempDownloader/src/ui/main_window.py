"""
主窗口模块

Ultra-TM02超低温温度测量模块上位机软件主界面
"""

from PyQt5.QtWidgets import (
    QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QGroupBox, QLabel, QComboBox, QPushButton, QLineEdit,
    QRadioButton, QButtonGroup, QMessageBox, QStatusBar,
    QFrame, QGridLayout, QFileDialog, QProgressDialog
)
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QFont, QDoubleValidator

from ..protocol.simulator import SimulatorProtocol
from ..protocol.commands import DeviceAPI
from ..utils.table_parser import TableParser


class MainWindow(QMainWindow):
    """主窗口类"""
    
    def __init__(self):
        """初始化主窗口"""
        super().__init__()
        
        # 初始化协议和API (使用模拟器协议，支持模拟设备测试)
        self.protocol = SimulatorProtocol()
        self.api = DeviceAPI(self.protocol)
        
        # 定时器
        self.refresh_timer = QTimer()
        self.refresh_timer.timeout.connect(self.on_refresh_timer)
        
        # 初始化UI
        self.init_ui()
        
        # 刷新串口列表
        self.refresh_ports()
    
    def init_ui(self):
        """初始化用户界面"""
        # 窗口设置
        self.setWindowTitle("超低温温度计 - Ultra-TM02")
        self.setMinimumSize(600, 500)
        self.setFont(QFont("Microsoft YaHei", 9))
        
        # 中央部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 主布局
        main_layout = QVBoxLayout(central_widget)
        main_layout.setSpacing(10)
        main_layout.setContentsMargins(15, 15, 15, 15)
        
        # 连接区域
        main_layout.addWidget(self.create_connection_group())
        
        # 电流源和4-20mA设置区域
        settings_layout = QHBoxLayout()
        settings_layout.addWidget(self.create_current_source_group())
        settings_layout.addWidget(self.create_420ma_group())
        main_layout.addLayout(settings_layout)
        
        # 操作按钮区域
        main_layout.addWidget(self.create_operation_group())
        
        # 数据显示区域
        main_layout.addWidget(self.create_data_group())
        
        # 状态栏
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("就绪")
    
    def create_connection_group(self) -> QGroupBox:
        """创建连接设置组"""
        group = QGroupBox("设备连接")
        layout = QGridLayout(group)
        
        # 串口选择
        layout.addWidget(QLabel("串行口:"), 0, 0)
        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(200)
        layout.addWidget(self.port_combo, 0, 1)
        
        # 刷新按钮
        self.refresh_btn = QPushButton("刷新")
        self.refresh_btn.clicked.connect(self.refresh_ports)
        layout.addWidget(self.refresh_btn, 0, 2)
        
        # 连接按钮
        self.connect_btn = QPushButton("连接")
        self.connect_btn.clicked.connect(self.on_connect_clicked)
        layout.addWidget(self.connect_btn, 0, 3)
        
        # 设备ID
        layout.addWidget(QLabel("设备ID:"), 1, 0)
        self.device_id_edit = QLineEdit()
        self.device_id_edit.setReadOnly(True)
        layout.addWidget(self.device_id_edit, 1, 1)
        
        # 获取设备ID按钮
        self.get_id_btn = QPushButton("获取设备ID")
        self.get_id_btn.clicked.connect(self.on_get_device_id)
        self.get_id_btn.setEnabled(False)
        layout.addWidget(self.get_id_btn, 1, 2, 1, 2)
        
        return group
    
    def create_current_source_group(self) -> QGroupBox:
        """创建电流源设置组"""
        group = QGroupBox("电流源")
        layout = QVBoxLayout(group)
        
        # 电流源选择
        self.current_src_group = QButtonGroup()
        self.radio_10ua = QRadioButton("10μA")
        self.radio_17ua = QRadioButton("17μA")
        self.radio_10ua.setChecked(True)
        self.current_src_group.addButton(self.radio_10ua, 0)
        self.current_src_group.addButton(self.radio_17ua, 1)
        
        layout.addWidget(self.radio_10ua)
        layout.addWidget(self.radio_17ua)
        
        # 分隔线
        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        layout.addWidget(line)
        
        # 10μA调整值
        adj_layout1 = QHBoxLayout()
        adj_layout1.addWidget(QLabel("10μA调整值:"))
        self.adj_10ua_edit = QLineEdit("0.0")
        self.adj_10ua_edit.setValidator(QDoubleValidator(-1.0, 1.0, 4))
        adj_layout1.addWidget(self.adj_10ua_edit)
        adj_layout1.addWidget(QLabel("μA"))
        layout.addLayout(adj_layout1)
        
        # 17μA调整值
        adj_layout2 = QHBoxLayout()
        adj_layout2.addWidget(QLabel("17μA调整值:"))
        self.adj_17ua_edit = QLineEdit("0.0")
        self.adj_17ua_edit.setValidator(QDoubleValidator(-1.0, 1.0, 4))
        adj_layout2.addWidget(self.adj_17ua_edit)
        adj_layout2.addWidget(QLabel("μA"))
        layout.addLayout(adj_layout2)
        
        # 电流源调整按钮
        self.current_adj_btn = QPushButton("电流源调整")
        self.current_adj_btn.clicked.connect(self.on_current_source_adjust)
        self.current_adj_btn.setEnabled(False)
        layout.addWidget(self.current_adj_btn)
        
        return group
    
    def create_420ma_group(self) -> QGroupBox:
        """创建4-20mA设置组"""
        group = QGroupBox("(4-20)mA")
        layout = QVBoxLayout(group)
        
        # 4mA温度点
        temp4_layout = QHBoxLayout()
        temp4_layout.addWidget(QLabel("4mA代表温度点:"))
        self.temp_4ma_edit = QLineEdit("-271.0")
        self.temp_4ma_edit.setValidator(QDoubleValidator(-300.0, 300.0, 3))
        temp4_layout.addWidget(self.temp_4ma_edit)
        temp4_layout.addWidget(QLabel("℃"))
        layout.addLayout(temp4_layout)
        
        # 20mA温度点
        temp20_layout = QHBoxLayout()
        temp20_layout.addWidget(QLabel("20mA代表温度点:"))
        self.temp_20ma_edit = QLineEdit("227.0")
        self.temp_20ma_edit.setValidator(QDoubleValidator(-300.0, 300.0, 3))
        temp20_layout.addWidget(self.temp_20ma_edit)
        temp20_layout.addWidget(QLabel("℃"))
        layout.addLayout(temp20_layout)
        
        layout.addStretch()
        
        # 4-20mA调整按钮
        self.ma_adj_btn = QPushButton("(4-20)mA调整")
        self.ma_adj_btn.clicked.connect(self.on_420ma_adjust)
        self.ma_adj_btn.setEnabled(False)
        layout.addWidget(self.ma_adj_btn)
        
        return group
    
    def create_operation_group(self) -> QGroupBox:
        """创建操作按钮组"""
        group = QGroupBox("操作")
        layout = QHBoxLayout(group)
        
        # 开始采集
        self.start_btn = QPushButton("开始采集")
        self.start_btn.clicked.connect(self.on_start_acq)
        self.start_btn.setEnabled(False)
        layout.addWidget(self.start_btn)
        
        # 停止采集
        self.stop_btn = QPushButton("停止采集")
        self.stop_btn.clicked.connect(self.on_stop_acq)
        self.stop_btn.setEnabled(False)
        layout.addWidget(self.stop_btn)
        
        # 加载分度表
        self.load_table_btn = QPushButton("加载分度表")
        self.load_table_btn.clicked.connect(self.on_load_table)
        self.load_table_btn.setEnabled(False)
        layout.addWidget(self.load_table_btn)
        
        # 存储参数
        self.save_param_btn = QPushButton("存储参数")
        self.save_param_btn.clicked.connect(self.on_save_param)
        self.save_param_btn.setEnabled(False)
        layout.addWidget(self.save_param_btn)
        
        return group
    
    def create_data_group(self) -> QGroupBox:
        """创建数据显示组"""
        group = QGroupBox("实时数据")
        layout = QGridLayout(group)
        
        # 温度显示
        layout.addWidget(QLabel("温度:"), 0, 0)
        self.temp_label = QLabel("--- ℃")
        self.temp_label.setFont(QFont("Microsoft YaHei", 18, QFont.Bold))
        self.temp_label.setStyleSheet("color: #0066CC;")
        layout.addWidget(self.temp_label, 0, 1)
        
        # 电压显示
        layout.addWidget(QLabel("电压:"), 1, 0)
        self.volt_label = QLabel("--- mV")
        self.volt_label.setFont(QFont("Microsoft YaHei", 14))
        layout.addWidget(self.volt_label, 1, 1)
        
        # 输出电流显示
        layout.addWidget(QLabel("输出电流:"), 2, 0)
        self.current_label = QLabel("--- mA")
        self.current_label.setFont(QFont("Microsoft YaHei", 14))
        layout.addWidget(self.current_label, 2, 1)
        
        # 状态显示
        layout.addWidget(QLabel("状态:"), 3, 0)
        self.status_label = QLabel("未连接")
        layout.addWidget(self.status_label, 3, 1)
        
        return group
    
    def refresh_ports(self):
        """刷新串口列表"""
        self.port_combo.clear()
        ports = SimulatorProtocol.list_ports()
        self.port_combo.addItems(ports)
        
        if ports:
            self.statusBar.showMessage(f"发现 {len(ports)} 个端口 (第一个为模拟设备)")
        else:
            self.statusBar.showMessage("未发现可用串口")
    
    def on_connect_clicked(self):
        """连接/断开按钮点击"""
        if self.protocol.connected:
            # 断开连接
            self.refresh_timer.stop()
            self.protocol.disconnect()
            self.connect_btn.setText("连接")
            self.set_controls_enabled(False)
            self.statusBar.showMessage("已断开连接")
            self.device_id_edit.clear()
            self.status_label.setText("未连接")
        else:
            # 连接
            port = self.port_combo.currentText()
            if not port:
                QMessageBox.warning(self, "警告", "请选择串口")
                return
                
            if self.protocol.connect(port):
                self.connect_btn.setText("断开")
                self.set_controls_enabled(True)
                self.statusBar.showMessage(f"已连接到 {port}")
                self.status_label.setText("已连接")
            else:
                QMessageBox.critical(self, "错误", f"无法连接到 {port}")
    
    def set_controls_enabled(self, enabled: bool):
        """设置控件启用状态"""
        self.get_id_btn.setEnabled(enabled)
        self.current_adj_btn.setEnabled(enabled)
        self.ma_adj_btn.setEnabled(enabled)
        self.start_btn.setEnabled(enabled)
        self.stop_btn.setEnabled(enabled)
        self.load_table_btn.setEnabled(enabled)
        self.save_param_btn.setEnabled(enabled)
    
    def on_get_device_id(self):
        """获取设备ID"""
        device_id = self.api.get_device_id()
        if device_id:
            self.device_id_edit.setText(device_id)
            self.statusBar.showMessage(f"设备ID: {device_id}")
        else:
            QMessageBox.warning(self, "警告", "获取设备ID失败")
    
    def on_current_source_adjust(self):
        """电流源调整"""
        # 设置电流源选择
        source = self.current_src_group.checkedId()
        if not self.api.set_current_source(source):
            QMessageBox.warning(self, "警告", "设置电流源失败")
            return
        
        # 设置调整值
        try:
            adj_10 = float(self.adj_10ua_edit.text())
            adj_17 = float(self.adj_17ua_edit.text())
        except ValueError:
            QMessageBox.warning(self, "警告", "请输入有效的调整值")
            return
        
        if not self.api.set_current_adj_10(adj_10):
            QMessageBox.warning(self, "警告", "设置10μA调整值失败")
            return
            
        if not self.api.set_current_adj_17(adj_17):
            QMessageBox.warning(self, "警告", "设置17μA调整值失败")
            return
        
        self.statusBar.showMessage("电流源设置成功")
    
    def on_420ma_adjust(self):
        """4-20mA调整"""
        try:
            temp_4ma = float(self.temp_4ma_edit.text())
            temp_20ma = float(self.temp_20ma_edit.text())
        except ValueError:
            QMessageBox.warning(self, "警告", "请输入有效的温度值")
            return
        
        if not self.api.set_4ma_temp(temp_4ma):
            QMessageBox.warning(self, "警告", "设置4mA温度点失败")
            return
            
        if not self.api.set_20ma_temp(temp_20ma):
            QMessageBox.warning(self, "警告", "设置20mA温度点失败")
            return
        
        self.statusBar.showMessage("4-20mA设置成功")
    
    def on_start_acq(self):
        """开始采集"""
        if self.api.start_acquisition():
            self.statusBar.showMessage("采集已开始")
            self.status_label.setText("采集中")
            # 启动刷新定时器 (1秒)
            self.refresh_timer.start(1000)
        else:
            QMessageBox.warning(self, "警告", "启动采集失败")
    
    def on_stop_acq(self):
        """停止采集"""
        self.refresh_timer.stop()
        if self.api.stop_acquisition():
            self.statusBar.showMessage("采集已停止")
            self.status_label.setText("已停止")
        else:
            QMessageBox.warning(self, "警告", "停止采集失败")
    
    def on_load_table(self):
        """加载分度表"""
        filename, _ = QFileDialog.getOpenFileName(
            self, "选择分度表文件", "", "CSV文件 (*.csv);;所有文件 (*)"
        )
        if not filename:
            return
        
        # 解析分度表文件
        parser = TableParser()
        if not parser.load_csv(filename):
            QMessageBox.warning(self, "警告", "分度表文件解析失败")
            return
        
        point_count = parser.get_point_count()
        
        # 确认下载
        reply = QMessageBox.question(
            self, "确认下载",
            f"分度表包含 {point_count} 个数据点\n确定要下载到设备吗？",
            QMessageBox.Yes | QMessageBox.No,
            QMessageBox.Yes
        )
        
        if reply != QMessageBox.Yes:
            return
        
        # 创建进度对话框
        progress = QProgressDialog("正在下载分度表...", "取消", 0, 100, self)
        progress.setWindowTitle("分度表下载")
        progress.setWindowModality(Qt.WindowModal)
        progress.setMinimumDuration(0)
        progress.setValue(0)
        
        # 进度回调
        def update_progress(current, total):
            percent = int(current * 100 / total)
            progress.setValue(percent)
            progress.setLabelText(f"正在下载... {current}/{total} 包")
            # 处理事件，保持UI响应
            from PyQt5.QtWidgets import QApplication
            QApplication.processEvents()
        
        # 执行下载
        success, message = self.api.download_table(parser, update_progress)
        
        progress.close()
        
        if success:
            self.statusBar.showMessage(f"分度表下载成功: {point_count}点")
            QMessageBox.information(self, "成功", message)
        else:
            QMessageBox.warning(self, "下载失败", message)
    
    def on_save_param(self):
        """保存参数"""
        if self.api.save_param():
            self.statusBar.showMessage("参数已保存")
            QMessageBox.information(self, "成功", "参数保存成功")
        else:
            QMessageBox.warning(self, "警告", "参数保存失败")
    
    def on_refresh_timer(self):
        """刷新定时器回调"""
        # 获取温度
        temp = self.api.get_temperature()
        if temp is not None:
            self.temp_label.setText(f"{temp:.3f} ℃")
        
        # 获取电压
        volt = self.api.get_voltage()
        if volt is not None:
            self.volt_label.setText(f"{volt:.3f} mV")
        
        # 获取电流
        current = self.api.get_current()
        if current is not None:
            self.current_label.setText(f"{current:.2f} mA")
    
    def closeEvent(self, event):
        """关闭窗口事件"""
        self.refresh_timer.stop()
        if self.protocol.connected:
            self.protocol.disconnect()
        event.accept()

