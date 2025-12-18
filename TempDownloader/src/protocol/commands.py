"""
命令定义模块

定义与Ultra-TM02设备通讯的所有命令
"""

import struct
from typing import Optional, Tuple
from .protocol import Protocol, Frame


class Commands:
    """命令码定义"""
    
    # 查询命令
    GET_DEVICE_ID       = 0x01      # 获取设备ID
    GET_TEMPERATURE     = 0x02      # 获取温度值
    GET_VOLTAGE         = 0x03      # 获取电压值
    GET_CURRENT         = 0x04      # 获取输出电流
    GET_STATUS          = 0x05      # 获取设备状态
    
    # 电流源设置
    SET_CURRENT_SRC     = 0x10      # 设置电流源
    SET_CURRENT_ADJ_10  = 0x11      # 设置10μA调整值
    SET_CURRENT_ADJ_17  = 0x12      # 设置17μA调整值
    
    # 4-20mA设置
    SET_4MA_TEMP        = 0x20      # 设置4mA温度点
    SET_20MA_TEMP       = 0x21      # 设置20mA温度点
    
    # 采集控制
    START_ACQ           = 0x30      # 开始采集
    STOP_ACQ            = 0x31      # 停止采集
    
    # 分度表下载
    LOAD_TABLE_START    = 0x40      # 分度表下载开始
    LOAD_TABLE_DATA     = 0x41      # 分度表数据包
    LOAD_TABLE_END      = 0x42      # 分度表下载结束
    
    # 参数管理
    SAVE_PARAM          = 0x50      # 保存参数
    LOAD_PARAM          = 0x51      # 加载参数
    RESET_DEFAULT       = 0x52      # 恢复默认
    
    # 响应
    ACK                 = 0x80      # 确认响应
    NACK                = 0x81      # 否定响应
    DATA_REPORT         = 0xF0      # 数据上报


class StatusCode:
    """状态码定义"""
    OK              = 0x00      # 成功
    INVALID_CMD     = 0x01      # 命令无效
    INVALID_PARAM   = 0x02      # 参数错误
    CRC_ERROR       = 0x03      # CRC错误
    BUSY            = 0x04      # 忙
    FLASH_ERROR     = 0x05      # Flash写入失败
    TABLE_ERROR     = 0x06      # 分度表错误


class DeviceAPI:
    """设备API封装类"""
    
    def __init__(self, protocol: Protocol):
        """
        初始化API
        
        Args:
            protocol: 协议处理器实例
        """
        self.protocol = protocol
    
    def get_device_id(self) -> Optional[str]:
        """
        获取设备ID
        
        Returns:
            设备ID字符串，失败返回None
        """
        response = self.protocol.send_command(Commands.GET_DEVICE_ID)
        if response and response.cmd == Commands.GET_DEVICE_ID:
            return response.data.decode('utf-8').rstrip('\x00')
        return None
    
    def get_temperature(self) -> Optional[float]:
        """
        获取温度值
        
        Returns:
            温度值(℃)，失败返回None
        """
        response = self.protocol.send_command(Commands.GET_TEMPERATURE)
        if response and response.cmd == Commands.GET_TEMPERATURE and len(response.data) >= 4:
            return struct.unpack('<f', response.data[:4])[0]
        return None
    
    def get_voltage(self) -> Optional[float]:
        """
        获取电压值
        
        Returns:
            电压值(mV)，失败返回None
        """
        response = self.protocol.send_command(Commands.GET_VOLTAGE)
        if response and response.cmd == Commands.GET_VOLTAGE and len(response.data) >= 4:
            return struct.unpack('<f', response.data[:4])[0]
        return None
    
    def get_current(self) -> Optional[float]:
        """
        获取输出电流
        
        Returns:
            电流值(mA)，失败返回None
        """
        response = self.protocol.send_command(Commands.GET_CURRENT)
        if response and response.cmd == Commands.GET_CURRENT and len(response.data) >= 4:
            return struct.unpack('<f', response.data[:4])[0]
        return None
    
    def get_status(self) -> Optional[dict]:
        """
        获取设备状态
        
        Returns:
            状态字典，失败返回None
        """
        response = self.protocol.send_command(Commands.GET_STATUS)
        if response and response.cmd == Commands.GET_STATUS and len(response.data) >= 8:
            running, current_src, probe_status, _, sample_count = struct.unpack('<BBBBI', response.data[:8])
            return {
                'running': bool(running),
                'current_source': current_src,
                'probe_status': probe_status,
                'sample_count': sample_count
            }
        return None
    
    def set_current_source(self, source: int) -> bool:
        """
        设置电流源
        
        Args:
            source: 0=10μA, 1=17μA
            
        Returns:
            是否成功
        """
        data = bytes([source & 0x01])
        response = self.protocol.send_command(Commands.SET_CURRENT_SRC, data)
        return self._check_ack(response)
    
    def set_current_adj_10(self, adj: float) -> bool:
        """
        设置10μA调整值
        
        Args:
            adj: 调整值(μA)
            
        Returns:
            是否成功
        """
        data = struct.pack('<f', adj)
        response = self.protocol.send_command(Commands.SET_CURRENT_ADJ_10, data)
        return self._check_ack(response)
    
    def set_current_adj_17(self, adj: float) -> bool:
        """
        设置17μA调整值
        
        Args:
            adj: 调整值(μA)
            
        Returns:
            是否成功
        """
        data = struct.pack('<f', adj)
        response = self.protocol.send_command(Commands.SET_CURRENT_ADJ_17, data)
        return self._check_ack(response)
    
    def set_4ma_temp(self, temp: float) -> bool:
        """
        设置4mA温度点
        
        Args:
            temp: 温度值(℃)
            
        Returns:
            是否成功
        """
        data = struct.pack('<f', temp)
        response = self.protocol.send_command(Commands.SET_4MA_TEMP, data)
        return self._check_ack(response)
    
    def set_20ma_temp(self, temp: float) -> bool:
        """
        设置20mA温度点
        
        Args:
            temp: 温度值(℃)
            
        Returns:
            是否成功
        """
        data = struct.pack('<f', temp)
        response = self.protocol.send_command(Commands.SET_20MA_TEMP, data)
        return self._check_ack(response)
    
    def start_acquisition(self) -> bool:
        """
        开始采集
        
        Returns:
            是否成功
        """
        response = self.protocol.send_command(Commands.START_ACQ)
        return self._check_ack(response)
    
    def stop_acquisition(self) -> bool:
        """
        停止采集
        
        Returns:
            是否成功
        """
        response = self.protocol.send_command(Commands.STOP_ACQ)
        return self._check_ack(response)
    
    def save_param(self) -> bool:
        """
        保存参数到Flash
        
        Returns:
            是否成功
        """
        response = self.protocol.send_command(Commands.SAVE_PARAM)
        return self._check_ack(response)
    
    def load_param(self) -> bool:
        """
        从Flash加载参数
        
        Returns:
            是否成功
        """
        response = self.protocol.send_command(Commands.LOAD_PARAM)
        return self._check_ack(response)
    
    def reset_default(self) -> bool:
        """
        恢复默认参数
        
        Returns:
            是否成功
        """
        response = self.protocol.send_command(Commands.RESET_DEFAULT)
        return self._check_ack(response)
    
    def load_table_start(self, point_count: int) -> bool:
        """
        分度表下载开始
        
        Args:
            point_count: 数据点数量
            
        Returns:
            是否成功
        """
        data = struct.pack('<H', point_count)
        response = self.protocol.send_command(Commands.LOAD_TABLE_START, data)
        return self._check_ack(response)
    
    def load_table_data(self, packet_index: int, packet_data: bytes) -> bool:
        """
        发送分度表数据包
        
        Args:
            packet_index: 包序号
            packet_data: 数据包内容
            
        Returns:
            是否成功
        """
        data = struct.pack('<H', packet_index) + packet_data
        response = self.protocol.send_command(Commands.LOAD_TABLE_DATA, data)
        return self._check_ack(response)
    
    def load_table_end(self) -> bool:
        """
        分度表下载结束
        
        Returns:
            是否成功
        """
        response = self.protocol.send_command(Commands.LOAD_TABLE_END)
        return self._check_ack(response)
    
    def download_table(self, table_parser, progress_callback=None) -> Tuple[bool, str]:
        """
        下载完整分度表
        
        Args:
            table_parser: 分度表解析器实例
            progress_callback: 进度回调函数 (current, total)
            
        Returns:
            (是否成功, 消息)
        """
        from loguru import logger
        
        point_count = table_parser.get_point_count()
        if point_count == 0:
            return False, "分度表为空"
        
        # 1. 发送开始命令
        if not self.load_table_start(point_count):
            return False, "发送开始命令失败"
        
        logger.info(f"开始下载分度表，共{point_count}个数据点")
        
        # 2. 分包发送数据
        packets = table_parser.get_packets(points_per_packet=32)
        total_packets = len(packets)
        
        for i, (packet_index, packet_data) in enumerate(packets):
            if not self.load_table_data(packet_index, packet_data):
                return False, f"发送数据包{packet_index}失败"
            
            # 进度回调
            if progress_callback:
                progress_callback(i + 1, total_packets)
        
        # 3. 发送结束命令
        if not self.load_table_end():
            return False, "发送结束命令失败"
        
        logger.info(f"分度表下载完成，共发送{total_packets}个数据包")
        return True, f"下载成功，共{point_count}个数据点"
    
    def _check_ack(self, response: Optional[Frame]) -> bool:
        """检查响应是否成功"""
        if response and response.cmd == Commands.ACK:
            if len(response.data) >= 2:
                status = response.data[1]
                return status == StatusCode.OK
        return False

