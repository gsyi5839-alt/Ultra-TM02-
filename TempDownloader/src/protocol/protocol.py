"""
通讯协议实现模块

提供与Ultra-TM02设备的串口通讯协议
"""

import struct
from dataclasses import dataclass
from typing import Optional, List
import serial
import serial.tools.list_ports
from loguru import logger


# 帧定义
FRAME_HEAD = 0xAA
FRAME_TAIL = 0x55


@dataclass
class Frame:
    """通讯帧数据结构"""
    cmd: int = 0            # 命令码
    data: bytes = b''       # 数据
    
    def to_bytes(self) -> bytes:
        """将帧转换为字节序列"""
        # 计算CRC
        crc_data = bytes([self.cmd, len(self.data)]) + self.data
        crc = Protocol.crc16(crc_data)
        
        # 组装帧
        frame = bytes([FRAME_HEAD, self.cmd, len(self.data)])
        frame += self.data
        frame += struct.pack('<H', crc)
        frame += bytes([FRAME_TAIL])
        
        return frame
    
    @staticmethod
    def from_bytes(data: bytes) -> Optional['Frame']:
        """从字节序列解析帧"""
        if len(data) < 6:
            return None
            
        if data[0] != FRAME_HEAD or data[-1] != FRAME_TAIL:
            return None
            
        cmd = data[1]
        length = data[2]
        
        if len(data) < 6 + length:
            return None
            
        payload = data[3:3+length]
        crc_received = struct.unpack('<H', data[3+length:5+length])[0]
        
        # 验证CRC
        crc_data = bytes([cmd, length]) + payload
        crc_calc = Protocol.crc16(crc_data)
        
        if crc_received != crc_calc:
            logger.warning(f"CRC校验失败: 接收={crc_received:04X}, 计算={crc_calc:04X}")
            return None
            
        return Frame(cmd=cmd, data=payload)


class Protocol:
    """通讯协议处理类"""
    
    def __init__(self):
        """初始化协议处理器"""
        self.serial: Optional[serial.Serial] = None
        self.connected = False
        self.rx_buffer = bytearray()
        
    @staticmethod
    def crc16(data: bytes) -> int:
        """
        CRC16计算 (CCITT/Modbus)
        
        Args:
            data: 输入数据
            
        Returns:
            16位CRC值
        """
        crc = 0xFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x0001:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc
    
    @staticmethod
    def list_ports() -> List[str]:
        """
        列出所有可用串口
        
        Returns:
            串口名称列表
        """
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]
    
    def connect(self, port: str, baudrate: int = 115200) -> bool:
        """
        连接设备
        
        Args:
            port: 串口名称
            baudrate: 波特率
            
        Returns:
            是否连接成功
        """
        try:
            self.serial = serial.Serial(
                port=port,
                baudrate=baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1.0
            )
            self.connected = True
            logger.info(f"已连接到 {port}")
            return True
        except Exception as e:
            logger.error(f"连接失败: {e}")
            self.connected = False
            return False
    
    def disconnect(self):
        """断开连接"""
        if self.serial and self.serial.is_open:
            self.serial.close()
        self.connected = False
        self.serial = None
        logger.info("已断开连接")
    
    def send_frame(self, frame: Frame) -> bool:
        """
        发送帧
        
        Args:
            frame: 要发送的帧
            
        Returns:
            是否发送成功
        """
        if not self.connected or not self.serial:
            logger.warning("未连接设备")
            return False
            
        try:
            data = frame.to_bytes()
            self.serial.write(data)
            logger.debug(f"发送: {data.hex().upper()}")
            return True
        except Exception as e:
            logger.error(f"发送失败: {e}")
            return False
    
    def receive_frame(self, timeout: float = 1.0) -> Optional[Frame]:
        """
        接收帧
        
        Args:
            timeout: 超时时间(秒)
            
        Returns:
            接收到的帧，超时返回None
        """
        if not self.connected or not self.serial:
            return None
            
        self.serial.timeout = timeout
        
        try:
            # 读取数据
            while True:
                byte = self.serial.read(1)
                if not byte:
                    break
                    
                self.rx_buffer.append(byte[0])
                
                # 查找帧头
                while len(self.rx_buffer) > 0 and self.rx_buffer[0] != FRAME_HEAD:
                    self.rx_buffer.pop(0)
                
                # 检查帧完整性
                if len(self.rx_buffer) >= 6:
                    length = self.rx_buffer[2] if len(self.rx_buffer) > 2 else 0
                    frame_len = 6 + length
                    
                    if len(self.rx_buffer) >= frame_len:
                        frame_data = bytes(self.rx_buffer[:frame_len])
                        self.rx_buffer = self.rx_buffer[frame_len:]
                        
                        frame = Frame.from_bytes(frame_data)
                        if frame:
                            logger.debug(f"接收: {frame_data.hex().upper()}")
                            return frame
                            
        except Exception as e:
            logger.error(f"接收失败: {e}")
            
        return None
    
    def send_command(self, cmd: int, data: bytes = b'', 
                     wait_response: bool = True) -> Optional[Frame]:
        """
        发送命令并等待响应
        
        Args:
            cmd: 命令码
            data: 命令数据
            wait_response: 是否等待响应
            
        Returns:
            响应帧，不等待响应或超时返回None
        """
        frame = Frame(cmd=cmd, data=data)
        
        if not self.send_frame(frame):
            return None
            
        if wait_response:
            return self.receive_frame()
            
        return None

