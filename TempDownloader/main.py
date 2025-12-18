"""
TempDownloader - Ultra-TM02 超低温温度测量模块上位机软件

主程序入口

版本: V1.0
日期: 2025-12-18
"""

import sys
from loguru import logger
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import Qt

from src.ui import MainWindow


def setup_logging():
    """配置日志"""
    logger.remove()  # 移除默认处理器
    logger.add(
        sys.stderr,
        format="<green>{time:YYYY-MM-DD HH:mm:ss}</green> | "
               "<level>{level: <8}</level> | "
               "<cyan>{name}</cyan>:<cyan>{function}</cyan>:<cyan>{line}</cyan> | "
               "<level>{message}</level>",
        level="DEBUG"
    )
    logger.add(
        "logs/app_{time:YYYY-MM-DD}.log",
        rotation="1 day",
        retention="7 days",
        level="INFO",
        encoding="utf-8"
    )


def main():
    """主函数"""
    # 配置日志
    setup_logging()
    logger.info("TempDownloader 启动")
    
    # 启用高DPI支持
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)
    QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps, True)
    
    # 创建应用
    app = QApplication(sys.argv)
    app.setStyle('Fusion')
    
    # 设置样式表
    app.setStyleSheet("""
        QMainWindow {
            background-color: #F5F5F5;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #CCCCCC;
            border-radius: 5px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        QPushButton {
            background-color: #0078D4;
            color: white;
            border: none;
            padding: 8px 16px;
            border-radius: 4px;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #106EBE;
        }
        QPushButton:pressed {
            background-color: #005A9E;
        }
        QPushButton:disabled {
            background-color: #CCCCCC;
            color: #888888;
        }
        QLineEdit {
            padding: 6px;
            border: 1px solid #CCCCCC;
            border-radius: 4px;
        }
        QLineEdit:focus {
            border-color: #0078D4;
        }
        QComboBox {
            padding: 6px;
            border: 1px solid #CCCCCC;
            border-radius: 4px;
        }
        QRadioButton {
            spacing: 8px;
        }
    """)
    
    # 创建主窗口
    window = MainWindow()
    window.show()
    
    # 运行
    ret = app.exec_()
    
    logger.info("TempDownloader 退出")
    return ret


if __name__ == '__main__':
    sys.exit(main())

