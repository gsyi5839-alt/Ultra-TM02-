"""
分度表解析模块

提供分度表文件的读取和解析功能
"""

import csv
import struct
from typing import List, Tuple, Optional
from dataclasses import dataclass
from loguru import logger


@dataclass
class TablePoint:
    """分度表数据点"""
    voltage: float      # 电压 (mV)
    temperature: float  # 温度 (K)


class TableParser:
    """分度表解析器"""
    
    # 分度表最大点数
    MAX_POINTS = 4871
    
    def __init__(self):
        """初始化解析器"""
        self.points: List[TablePoint] = []
    
    def load_csv(self, filename: str) -> bool:
        """
        从CSV文件加载分度表
        
        Args:
            filename: CSV文件路径
            
        Returns:
            是否加载成功
        
        CSV格式:
            第一行: 标题 (Voltage_mV, Temperature_K)
            后续行: 电压值, 温度值
        """
        self.points.clear()
        
        try:
            with open(filename, 'r', encoding='utf-8') as f:
                reader = csv.reader(f)
                
                # 跳过标题行
                next(reader, None)
                
                for row in reader:
                    if len(row) >= 2:
                        try:
                            voltage = float(row[0])
                            temperature = float(row[1])
                            self.points.append(TablePoint(voltage, temperature))
                        except ValueError:
                            continue
                
                # 检查点数
                if len(self.points) > self.MAX_POINTS:
                    logger.warning(f"分度表点数超过最大限制，截断至{self.MAX_POINTS}点")
                    self.points = self.points[:self.MAX_POINTS]
                
                logger.info(f"加载分度表成功，共{len(self.points)}个数据点")
                return True
                
        except Exception as e:
            logger.error(f"加载分度表失败: {e}")
            return False
    
    def get_point_count(self) -> int:
        """获取数据点数量"""
        return len(self.points)
    
    def to_binary(self) -> bytes:
        """
        将分度表转换为二进制格式
        
        Returns:
            二进制数据
            
        格式:
            4字节: 魔数 "TBL\0" (0x004C4254)
            2字节: 点数 (uint16)
            2字节: 保留
            N * 8字节: 数据点 (voltage float + temperature float)
        """
        # 魔数
        magic = 0x004C4254
        
        # 组装数据
        data = struct.pack('<I', magic)
        data += struct.pack('<H', len(self.points))
        data += struct.pack('<H', 0)  # 保留
        
        # 数据点
        for point in self.points:
            data += struct.pack('<ff', point.voltage, point.temperature)
        
        return data
    
    def get_packets(self, points_per_packet: int = 32) -> List[Tuple[int, bytes]]:
        """
        将分度表分包
        
        Args:
            points_per_packet: 每包点数
            
        Returns:
            包列表 [(包序号, 数据), ...]
        """
        packets = []
        total_packets = (len(self.points) + points_per_packet - 1) // points_per_packet
        
        for i in range(total_packets):
            start = i * points_per_packet
            end = min(start + points_per_packet, len(self.points))
            
            # 包序号
            packet_data = struct.pack('<H', i)
            
            # 数据点
            for j in range(start, end):
                point = self.points[j]
                packet_data += struct.pack('<ff', point.voltage, point.temperature)
            
            packets.append((i, packet_data))
        
        return packets
    
    @staticmethod
    def create_sample_table(filename: str):
        """
        创建示例分度表文件
        
        Args:
            filename: 输出文件路径
        """
        # 生成示例数据 (简化的二极管V-T曲线)
        # 实际应使用真实的二极管分度表数据
        
        with open(filename, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow(['Voltage_mV', 'Temperature_K'])
            
            # 生成1.4K到500K的数据点
            for t in range(14, 5000):  # 1.4K到500K (0.1K步进)
                temp_k = t / 10.0
                
                # 简化的V-T关系 (实际应使用标准分度表)
                # 低温区: V ≈ 1600 - 2.5 * T (mV)
                # 高温区: V ≈ 1200 - 2.0 * T (mV)
                if temp_k < 77:
                    voltage = 1600 - 2.5 * temp_k
                else:
                    voltage = 1200 - 2.0 * temp_k
                
                writer.writerow([f'{voltage:.3f}', f'{temp_k:.1f}'])
        
        logger.info(f"示例分度表已创建: {filename}")

