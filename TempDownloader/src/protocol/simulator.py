"""
模拟设备模块

用于在没有真实硬件的情况下测试上位机软件
"""

import struct
import random
import time
from typing import Optional, List
from loguru import logger

from .protocol import Protocol, Frame, FRAME_HEAD, FRAME_TAIL
from .commands import Commands, StatusCode


class SimulatorProtocol(Protocol):
    """模拟设备协议类 - 用于测试"""
    
    def __init__(self):
        """初始化模拟器"""
        super().__init__()
        
        # 模拟设备状态
        self.sim_device_id = "ULTRA-TM02-SIM01"  # 模拟设备ID
        self.sim_temperature = 25.0              # 模拟温度 (℃)
        self.sim_voltage = 580.0                 # 模拟电压 (mV)
        self.sim_current = 12.0                  # 模拟输出电流 (mA)
        self.sim_running = False                 # 采集状态
        self.sim_current_source = 0             # 电流源 (0=10μA, 1=17μA)
        self.sim_adj_10 = 0.0                   # 10μA调整值
        self.sim_adj_17 = 0.0                   # 17μA调整值
        self.sim_temp_4ma = -271.0              # 4mA温度点
        self.sim_temp_20ma = 227.0              # 20mA温度点
        
        logger.info("模拟设备协议已初始化")
    
    @staticmethod
    def list_ports() -> List[str]:
        """列出串口（添加模拟端口）"""
        ports = Protocol.list_ports()
        # 添加模拟端口到列表开头
        return ["[模拟设备]"] + ports
    
    def connect(self, port: str, baudrate: int = 115200) -> bool:
        """连接设备"""
        if port == "[模拟设备]":
            # 模拟连接
            self.connected = True
            self.serial = None  # 模拟模式不需要真实串口
            logger.info("已连接到模拟设备")
            return True
        else:
            # 真实连接
            return super().connect(port, baudrate)
    
    def disconnect(self):
        """断开连接"""
        if self.serial is None and self.connected:
            # 模拟断开
            self.connected = False
            self.sim_running = False
            logger.info("已断开模拟设备")
        else:
            super().disconnect()
    
    def send_command(self, cmd: int, data: bytes = b'', 
                     wait_response: bool = True) -> Optional[Frame]:
        """发送命令并获取响应"""
        if self.serial is None and self.connected:
            # 模拟模式 - 生成模拟响应
            return self._simulate_response(cmd, data)
        else:
            # 真实模式
            return super().send_command(cmd, data, wait_response)
    
    def _simulate_response(self, cmd: int, data: bytes) -> Optional[Frame]:
        """生成模拟响应"""
        # 模拟通讯延迟
        time.sleep(0.05)
        
        logger.debug(f"模拟命令: 0x{cmd:02X}, 数据: {data.hex() if data else '无'}")
        
        # 根据命令生成响应
        if cmd == Commands.GET_DEVICE_ID:
            # 返回设备ID
            return Frame(cmd=cmd, data=self.sim_device_id.encode('utf-8'))
        
        elif cmd == Commands.GET_TEMPERATURE:
            # 返回温度值（添加随机波动）
            if self.sim_running:
                self.sim_temperature += random.uniform(-0.5, 0.5)
            temp_data = struct.pack('<f', self.sim_temperature)
            return Frame(cmd=cmd, data=temp_data)
        
        elif cmd == Commands.GET_VOLTAGE:
            # 返回电压值（添加随机波动）
            if self.sim_running:
                self.sim_voltage += random.uniform(-1.0, 1.0)
            volt_data = struct.pack('<f', self.sim_voltage)
            return Frame(cmd=cmd, data=volt_data)
        
        elif cmd == Commands.GET_CURRENT:
            # 计算输出电流
            self._update_output_current()
            current_data = struct.pack('<f', self.sim_current)
            return Frame(cmd=cmd, data=current_data)
        
        elif cmd == Commands.GET_STATUS:
            # 返回状态
            status_data = struct.pack('<BBBBI', 
                                      1 if self.sim_running else 0,
                                      self.sim_current_source,
                                      1,  # 探头状态正常
                                      0,  # 保留
                                      random.randint(1, 1000))  # 采样计数
            return Frame(cmd=cmd, data=status_data)
        
        elif cmd == Commands.SET_CURRENT_SRC:
            # 设置电流源
            if len(data) >= 1:
                self.sim_current_source = data[0]
                logger.info(f"模拟: 设置电流源 = {'17μA' if self.sim_current_source else '10μA'}")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.SET_CURRENT_ADJ_10:
            # 设置10μA调整值
            if len(data) >= 4:
                self.sim_adj_10 = struct.unpack('<f', data[:4])[0]
                logger.info(f"模拟: 设置10μA调整值 = {self.sim_adj_10}")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.SET_CURRENT_ADJ_17:
            # 设置17μA调整值
            if len(data) >= 4:
                self.sim_adj_17 = struct.unpack('<f', data[:4])[0]
                logger.info(f"模拟: 设置17μA调整值 = {self.sim_adj_17}")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.SET_4MA_TEMP:
            # 设置4mA温度点
            if len(data) >= 4:
                self.sim_temp_4ma = struct.unpack('<f', data[:4])[0]
                logger.info(f"模拟: 设置4mA温度点 = {self.sim_temp_4ma}℃")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.SET_20MA_TEMP:
            # 设置20mA温度点
            if len(data) >= 4:
                self.sim_temp_20ma = struct.unpack('<f', data[:4])[0]
                logger.info(f"模拟: 设置20mA温度点 = {self.sim_temp_20ma}℃")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.START_ACQ:
            # 开始采集
            self.sim_running = True
            logger.info("模拟: 开始采集")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.STOP_ACQ:
            # 停止采集
            self.sim_running = False
            logger.info("模拟: 停止采集")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.LOAD_TABLE_START:
            # 分度表开始
            if len(data) >= 2:
                point_count = struct.unpack('<H', data[:2])[0]
                logger.info(f"模拟: 分度表下载开始，{point_count}点")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.LOAD_TABLE_DATA:
            # 分度表数据
            if len(data) >= 2:
                packet_index = struct.unpack('<H', data[:2])[0]
                logger.debug(f"模拟: 接收分度表数据包 #{packet_index}")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.LOAD_TABLE_END:
            # 分度表结束
            logger.info("模拟: 分度表下载完成")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.SAVE_PARAM:
            # 保存参数
            logger.info("模拟: 参数已保存到Flash")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.LOAD_PARAM:
            # 加载参数
            logger.info("模拟: 参数已从Flash加载")
            return self._make_ack(cmd, StatusCode.OK)
        
        elif cmd == Commands.RESET_DEFAULT:
            # 恢复默认
            self._reset_defaults()
            logger.info("模拟: 已恢复默认参数")
            return self._make_ack(cmd, StatusCode.OK)
        
        else:
            logger.warning(f"模拟: 未知命令 0x{cmd:02X}")
            return self._make_ack(cmd, StatusCode.INVALID_CMD)
    
    def _make_ack(self, cmd: int, status: int) -> Frame:
        """生成ACK响应"""
        return Frame(cmd=Commands.ACK, data=bytes([cmd, status]))
    
    def _update_output_current(self):
        """更新输出电流"""
        # 根据温度计算4-20mA输出
        if self.sim_temp_20ma != self.sim_temp_4ma:
            ratio = (self.sim_temperature - self.sim_temp_4ma) / (self.sim_temp_20ma - self.sim_temp_4ma)
            self.sim_current = 4.0 + ratio * 16.0
            # 限幅
            self.sim_current = max(4.0, min(20.0, self.sim_current))
    
    def _reset_defaults(self):
        """恢复默认参数"""
        self.sim_current_source = 0
        self.sim_adj_10 = 0.0
        self.sim_adj_17 = 0.0
        self.sim_temp_4ma = -271.0
        self.sim_temp_20ma = 227.0
        self.sim_temperature = 25.0
        self.sim_voltage = 580.0

