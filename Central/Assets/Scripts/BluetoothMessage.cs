﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

/// <summary>
/// These message identifiers have to match up with the ones on the firmware of course!
/// </summary>
public enum DieMessageType : byte
{
    None = 0,
    State,
    Telemetry,
    BulkSetup,
    BulkSetupAck,
    BulkData,
    BulkDataAck,
    TransferAnimSet,
    TransferAnimSetAck,
    TransferAnimReadyForNextAnim,
    TransferSettings,
    TransferSettingsAck,
    DebugLog,

    PlayAnim,
    RequestState,
    RequestAnimSet,
    RequestSettings,
    RequestTelemetry,
}

public interface DieMessage
{
    DieMessageType type { get; set; }
}

public static class DieMessages
{
    public static DieMessage FromByteArray(byte[] data)
    {
        DieMessage ret = null;
        if (data.Length > 0)
        {
            DieMessageType type = (DieMessageType)data[0];
            switch (type)
            {
                case DieMessageType.State:
                    ret = FromByteArray<DieMessageState>(data);
                    break;
                case DieMessageType.Telemetry:
                    ret = FromByteArray<DieMessageAcc>(data);
                    break;
                case DieMessageType.BulkSetup:
                    ret = FromByteArray<DieMessageBulkSetup>(data);
                    break;
                case DieMessageType.BulkData:
                    ret = FromByteArray<DieMessageBulkData>(data);
                    break;
                case DieMessageType.BulkSetupAck:
                    ret = FromByteArray<DieMessageBulkSetupAck>(data);
                    break;
                case DieMessageType.BulkDataAck:
                    ret = FromByteArray<DieMessageBulkDataAck>(data);
                    break;
                case DieMessageType.TransferAnimSet:
                    ret = FromByteArray<DieMessageTransferAnimSet>(data);
                    break;
                case DieMessageType.TransferAnimSetAck:
                    ret = FromByteArray<DieMessageTransferAnimSetAck>(data);
                    break;
                case DieMessageType.TransferSettings:
                    ret = FromByteArray<DieMessageTransferSettings>(data);
                    break;
                case DieMessageType.TransferSettingsAck:
                    ret = FromByteArray<DieMessageTransferSettingsAck>(data);
                    break;
                case DieMessageType.DebugLog:
                    ret = FromByteArray<DieMessageDebugLog>(data);
                    break;
                case DieMessageType.PlayAnim:
                    ret = FromByteArray<DieMessagePlayAnim>(data);
                    break;
                case DieMessageType.RequestAnimSet:
                    ret = FromByteArray<DieMessageRequestAnimSet>(data);
                    break;
                case DieMessageType.RequestSettings:
                    ret = FromByteArray<DieMessageRequestSettings>(data);
                    break;
                case DieMessageType.RequestTelemetry:
                    ret = FromByteArray<DieMessageRequestTelemetry>(data);
                    break;
                default:
                    break;
            }
        }
        return ret;
    }

    static DieMessage FromByteArray<T>(byte[] data)
        where T : DieMessage
    {
        int size = Marshal.SizeOf(typeof(T));
        System.IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.Copy(data, 0, ptr, size);
        var retMessage = (T)Marshal.PtrToStructure(ptr, typeof(T));
        Marshal.FreeHGlobal(ptr);
        return retMessage;
    }

    // For virtual dice!
    public static byte[] ToByteArray<T>(T message)
        where T : DieMessage
    {
        int size = Marshal.SizeOf(typeof(T));
        System.IntPtr ptr = Marshal.AllocHGlobal(size);
        Marshal.StructureToPtr(message, ptr, false);
        byte[] ret = new byte[size];
        Marshal.Copy(ptr, ret, 0, size);
        Marshal.FreeHGlobal(ptr);
        return ret;
    }
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageState
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.State;
    public byte face;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageAcc
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.Telemetry;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 2)]
    public AccelFrame[] data;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageBulkSetup
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkSetup;
    public short size;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageBulkSetupAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkSetupAck;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageBulkData
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkData;
    public byte size;
    public short offset;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public byte[] data;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageBulkDataAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.BulkDataAck;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageTransferAnimSet
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferAnimSet;
    public byte count;
    public short totalAnimationByteSize;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageTransferAnimSetAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferAnimSetAck;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageTransferAnimReadyForNextAnim
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferAnimReadyForNextAnim;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageRequestAnimSet
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.RequestAnimSet;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageTransferSettings
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferSettings;
    public byte count;
    public short totalAnimationByteSize;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageTransferSettingsAck
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.TransferSettingsAck;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageRequestSettings
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.RequestSettings;
}


[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageRequestTelemetry
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.RequestTelemetry;
    public bool telemetry;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageDebugLog
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.DebugLog;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 19)]
    public byte[] data;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessagePlayAnim
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.PlayAnim;
    public byte index;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class DieMessageRequestState
    : DieMessage
{
    public DieMessageType type { get; set; } = DieMessageType.RequestState;
}

