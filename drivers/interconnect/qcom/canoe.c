// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 *
 */

#include <linux/device.h>
#include <linux/interconnect.h>
#include <linux/interconnect-provider.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <dt-bindings/interconnect/qcom,canoe-rpmh.h>

#include "bcm-voter.h"
#include "icc-common.h"
#include "icc-rpmh.h"

/*
 * This driver describes only the HLOS-voted Canoe interconnect fabrics.
 * Camera, display and PCIe CRM hardware-voter domains require CRM support
 * that is not available in the upstream RPMh interconnect provider.
 */

static const struct regmap_config icc_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
};

static const struct qcom_icc_qosbox qhm_qspi_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0xc000 },
};

static struct qcom_icc_node qhm_qspi = {
	.name = "qhm_qspi",
	.id = CANOE_MASTER_QSPI_0,
	.channels = 1,
	.buswidth = 4,
	.qosbox = &qhm_qspi_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A1NOC_SNOC },
};

static const struct qcom_icc_qosbox qxm_crypto_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x36000 },
};

static struct qcom_icc_node qxm_crypto = {
	.name = "qxm_crypto",
	.id = CANOE_MASTER_CRYPTO,
	.channels = 1,
	.buswidth = 16,
	.qosbox = &qxm_crypto_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A1NOC_SNOC },
};

static const struct qcom_icc_qosbox qxm_qup1_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x11000 },
};

static struct qcom_icc_node qxm_qup1 = {
	.name = "qxm_qup1",
	.id = CANOE_MASTER_QUP_1,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qxm_qup1_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A1NOC_SNOC },
};

static const struct qcom_icc_qosbox xm_sdc4_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0xe000 },
};

static struct qcom_icc_node xm_sdc4 = {
	.name = "xm_sdc4",
	.id = CANOE_MASTER_SDCC_4,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &xm_sdc4_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A1NOC_SNOC },
};

static const struct qcom_icc_qosbox xm_ufs_mem_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0xf000 },
};

static struct qcom_icc_node xm_ufs_mem = {
	.name = "xm_ufs_mem",
	.id = CANOE_MASTER_UFS_MEM,
	.channels = 1,
	.buswidth = 16,
	.qosbox = &xm_ufs_mem_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A1NOC_SNOC },
};

static const struct qcom_icc_qosbox xm_usb3_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x10000 },
};

static struct qcom_icc_node xm_usb3 = {
	.name = "xm_usb3",
	.id = CANOE_MASTER_USB3,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &xm_usb3_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A1NOC_SNOC },
};

static const struct qcom_icc_qosbox qhm_qup2_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x35000 },
};

static struct qcom_icc_node qhm_qup2 = {
	.name = "qhm_qup2",
	.id = CANOE_MASTER_QUP_2,
	.channels = 1,
	.buswidth = 4,
	.qosbox = &qhm_qup2_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox qhm_qup3_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x3c000 },
};

static struct qcom_icc_node qhm_qup3 = {
	.name = "qhm_qup3",
	.id = CANOE_MASTER_QUP_3,
	.channels = 1,
	.buswidth = 4,
	.qosbox = &qhm_qup3_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox qhm_qup4_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x3d000 },
};

static struct qcom_icc_node qhm_qup4 = {
	.name = "qhm_qup4",
	.id = CANOE_MASTER_QUP_4,
	.channels = 1,
	.buswidth = 4,
	.qosbox = &qhm_qup4_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox qxm_ipa_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x37000 },
};

static struct qcom_icc_node qxm_ipa = {
	.name = "qxm_ipa",
	.id = CANOE_MASTER_IPA,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qxm_ipa_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox qxm_soccp_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x3b000 },
};

static struct qcom_icc_node qxm_soccp = {
	.name = "qxm_soccp",
	.id = CANOE_MASTER_SOCCP_AGGR_NOC,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qxm_soccp_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static struct qcom_icc_node qxm_sp = {
	.name = "qxm_sp",
	.id = CANOE_MASTER_SP,
	.channels = 1,
	.buswidth = 8,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox xm_qdss_etr_0_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x38000 },
};

static struct qcom_icc_node xm_qdss_etr_0 = {
	.name = "xm_qdss_etr_0",
	.id = CANOE_MASTER_QDSS_ETR,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &xm_qdss_etr_0_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox xm_qdss_etr_1_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x39000 },
};

static struct qcom_icc_node xm_qdss_etr_1 = {
	.name = "xm_qdss_etr_1",
	.id = CANOE_MASTER_QDSS_ETR_1,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &xm_qdss_etr_1_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static const struct qcom_icc_qosbox xm_sdc2_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x3a000 },
};

static struct qcom_icc_node xm_sdc2 = {
	.name = "xm_sdc2",
	.id = CANOE_MASTER_SDCC_2,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &xm_sdc2_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_A2NOC_SNOC },
};

static struct qcom_icc_node qup0_core_master = {
	.name = "qup0_core_master",
	.id = CANOE_MASTER_QUP_CORE_0,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_QUP_CORE_0 },
};

static struct qcom_icc_node qup1_core_master = {
	.name = "qup1_core_master",
	.id = CANOE_MASTER_QUP_CORE_1,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_QUP_CORE_1 },
};

static struct qcom_icc_node qup2_core_master = {
	.name = "qup2_core_master",
	.id = CANOE_MASTER_QUP_CORE_2,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_QUP_CORE_2 },
};

static struct qcom_icc_node qup3_core_master = {
	.name = "qup3_core_master",
	.id = CANOE_MASTER_QUP_CORE_3,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_QUP_CORE_3 },
};

static struct qcom_icc_node qup4_core_master = {
	.name = "qup4_core_master",
	.id = CANOE_MASTER_QUP_CORE_4,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_QUP_CORE_4 },
};

static struct qcom_icc_node qsm_cfg = {
	.name = "qsm_cfg",
	.id = CANOE_MASTER_CNOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 35,
	.links = { CANOE_SLAVE_AHB2PHY_SOUTH, CANOE_SLAVE_AHB2PHY_NORTH,
		   CANOE_SLAVE_CAMERA_CFG, CANOE_SLAVE_CLK_CTL,
		   CANOE_SLAVE_CRYPTO_0_CFG, CANOE_SLAVE_DISPLAY_CFG,
		   CANOE_SLAVE_EVA_CFG, CANOE_SLAVE_GFX3D_CFG,
		   CANOE_SLAVE_I2C, CANOE_SLAVE_I3C_IBI0_CFG,
		   CANOE_SLAVE_I3C_IBI1_CFG, CANOE_SLAVE_IMEM_CFG,
		   CANOE_SLAVE_IPC_ROUTER_CFG, CANOE_SLAVE_CNOC_MSS,
		   CANOE_SLAVE_PCIE_CFG, CANOE_SLAVE_PRNG,
		   CANOE_SLAVE_QDSS_CFG, CANOE_SLAVE_QSPI_0,
		   CANOE_SLAVE_QUP_1, CANOE_SLAVE_QUP_2,
		   CANOE_SLAVE_QUP_3, CANOE_SLAVE_QUP_4,
		   CANOE_SLAVE_SDCC_2, CANOE_SLAVE_SDCC_4,
		   CANOE_SLAVE_SPSS_CFG, CANOE_SLAVE_TCSR,
		   CANOE_SLAVE_TLMM, CANOE_SLAVE_UFS_MEM_CFG,
		   CANOE_SLAVE_USB3, CANOE_SLAVE_VENUS_CFG,
		   CANOE_SLAVE_VSENSE_CTRL_CFG, CANOE_SLAVE_CNOC_MNOC_CFG,
		   CANOE_SLAVE_PCIE_ANOC_CFG, CANOE_SLAVE_QDSS_STM,
		   CANOE_SLAVE_TCU },
};

static struct qcom_icc_node qnm_gemnoc_cnoc = {
	.name = "qnm_gemnoc_cnoc",
	.id = CANOE_MASTER_GEM_NOC_CNOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 10,
	.links = { CANOE_SLAVE_AOSS, CANOE_SLAVE_IPA_CFG,
		   CANOE_SLAVE_IPC_ROUTER_FENCE, CANOE_SLAVE_SOCCP,
		   CANOE_SLAVE_TME_CFG, CANOE_SLAVE_APPSS,
		   CANOE_SLAVE_CNOC_CFG, CANOE_SLAVE_DDRSS_CFG,
		   CANOE_SLAVE_BOOT_IMEM, CANOE_SLAVE_IMEM },
};

static struct qcom_icc_node qnm_gemnoc_pcie = {
	.name = "qnm_gemnoc_pcie",
	.id = CANOE_MASTER_GEM_NOC_PCIE_SNOC,
	.channels = 1,
	.buswidth = 8,
	.num_links = 1,
	.links = { CANOE_SLAVE_PCIE_0 },
};

static const struct qcom_icc_qosbox alm_gpu_tcu_qos = {
	.prio = 1,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x13d000 },
};

static struct qcom_icc_node alm_gpu_tcu = {
	.name = "alm_gpu_tcu",
	.id = CANOE_MASTER_GPU_TCU,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &alm_gpu_tcu_qos,
	.num_links = 2,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC },
};

static const struct qcom_icc_qosbox alm_sys_tcu_qos = {
	.prio = 6,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x13f000 },
};

static struct qcom_icc_node alm_sys_tcu = {
	.name = "alm_sys_tcu",
	.id = CANOE_MASTER_SYS_TCU,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &alm_sys_tcu_qos,
	.num_links = 2,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC },
};

static struct qcom_icc_node chm_apps = {
	.name = "chm_apps",
	.id = CANOE_MASTER_APPSS_PROC,
	.channels = 4,
	.buswidth = 32,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox qnm_gpu_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 2,
	.port_offsets = { 0x31000, 0xb1000 },
};

static struct qcom_icc_node qnm_gpu = {
	.name = "qnm_gpu",
	.id = CANOE_MASTER_GFX3D,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_gpu_qos,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox qnm_lpass_gemnoc_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 1,
	.port_offsets = { 0x141000 },
};

static struct qcom_icc_node qnm_lpass_gemnoc = {
	.name = "qnm_lpass_gemnoc",
	.id = CANOE_MASTER_LPASS_GEM_NOC,
	.channels = 1,
	.buswidth = 16,
	.qosbox = &qnm_lpass_gemnoc_qos,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static struct qcom_icc_node qnm_mdsp = {
	.name = "qnm_mdsp",
	.id = CANOE_MASTER_MSS_PROC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox qnm_mnoc_hf_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x33000, 0xb3000 },
};

static struct qcom_icc_node qnm_mnoc_hf = {
	.name = "qnm_mnoc_hf",
	.id = CANOE_MASTER_MNOC_HF_MEM_NOC,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_mnoc_hf_qos,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox qnm_mnoc_sf_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x35000, 0xb5000 },
};

static struct qcom_icc_node qnm_mnoc_sf = {
	.name = "qnm_mnoc_sf",
	.id = CANOE_MASTER_MNOC_SF_MEM_NOC,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_mnoc_sf_qos,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox qnm_nsp_gemnoc_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 2,
	.port_offsets = { 0x37000, 0xb7000 },
};

static struct qcom_icc_node qnm_nsp_gemnoc = {
	.name = "qnm_nsp_gemnoc",
	.id = CANOE_MASTER_COMPUTE_NOC,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_nsp_gemnoc_qos,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox qnm_pcie_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 1,
	.port_offsets = { 0x143000 },
};

static struct qcom_icc_node qnm_pcie = {
	.name = "qnm_pcie",
	.id = CANOE_MASTER_ANOC_PCIE_GEM_NOC,
	.channels = 1,
	.buswidth = 16,
	.qosbox = &qnm_pcie_qos,
	.num_links = 2,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC },
};

static const struct qcom_icc_qosbox qnm_qpace_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 1,
	.port_offsets = { 0x14e000 },
};

static struct qcom_icc_node qnm_qpace = {
	.name = "qnm_qpace",
	.id = CANOE_MASTER_QPACE,
	.channels = 1,
	.buswidth = 32,
	.qosbox = &qnm_qpace_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_LLCC },
};

static const struct qcom_icc_qosbox qnm_snoc_sf_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 1,
	.port_offsets = { 0x147000 },
};

static struct qcom_icc_node qnm_snoc_sf = {
	.name = "qnm_snoc_sf",
	.id = CANOE_MASTER_SNOC_SF_MEM_NOC,
	.channels = 1,
	.buswidth = 16,
	.qosbox = &qnm_snoc_sf_qos,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static struct qcom_icc_node qnm_wlan_q6 = {
	.name = "qnm_wlan_q6",
	.id = CANOE_MASTER_WLAN_Q6,
	.channels = 1,
	.buswidth = 8,
	.num_links = 3,
	.links = { CANOE_SLAVE_GEM_NOC_CNOC, CANOE_SLAVE_LLCC,
		   CANOE_SLAVE_MEM_NOC_PCIE_SNOC },
};

static const struct qcom_icc_qosbox xm_gic_qos = {
	.prio = 4,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x145000 },
};

static struct qcom_icc_node xm_gic = {
	.name = "xm_gic",
	.id = CANOE_MASTER_GIC,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &xm_gic_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_LLCC },
};

static struct qcom_icc_node qnm_lpiaon_noc = {
	.name = "qnm_lpiaon_noc",
	.id = CANOE_MASTER_LPIAON_NOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_SLAVE_LPASS_GEM_NOC },
};

static struct qcom_icc_node qnm_lpass_lpinoc = {
	.name = "qnm_lpass_lpinoc",
	.id = CANOE_MASTER_LPASS_LPINOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_SLAVE_LPIAON_NOC_LPASS_AG_NOC },
};

static struct qcom_icc_node qnm_lpinoc_dsp_qns4m = {
	.name = "qnm_lpinoc_dsp_qns4m",
	.id = CANOE_MASTER_LPASS_PROC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_SLAVE_LPICX_NOC_LPIAON_NOC },
};

static struct qcom_icc_node llcc_mc = {
	.name = "llcc_mc",
	.id = CANOE_MASTER_LLCC,
	.channels = 4,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_EBI1 },
};

static const struct qcom_icc_qosbox qnm_camnoc_hf_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x2a000, 0x2b000 },
};

static struct qcom_icc_node qnm_camnoc_hf = {
	.name = "qnm_camnoc_hf",
	.id = CANOE_MASTER_CAMNOC_HF,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_camnoc_hf_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_HF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_camnoc_nrt_icp_sf_qos = {
	.prio = 4,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x2c000 },
};

static struct qcom_icc_node qnm_camnoc_nrt_icp_sf = {
	.name = "qnm_camnoc_nrt_icp_sf",
	.id = CANOE_MASTER_CAMNOC_NRT_ICP_SF,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_camnoc_nrt_icp_sf_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_camnoc_rt_cdm_sf_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x38000 },
};

static struct qcom_icc_node qnm_camnoc_rt_cdm_sf = {
	.name = "qnm_camnoc_rt_cdm_sf",
	.id = CANOE_MASTER_CAMNOC_RT_CDM_SF,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_camnoc_rt_cdm_sf_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_camnoc_sf_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x2d000, 0x2e000 },
};

static struct qcom_icc_node qnm_camnoc_sf = {
	.name = "qnm_camnoc_sf",
	.id = CANOE_MASTER_CAMNOC_SF,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_camnoc_sf_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_mdp_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x2f000, 0x30000 },
};

static struct qcom_icc_node qnm_mdp = {
	.name = "qnm_mdp",
	.id = CANOE_MASTER_MDP,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_mdp_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_HF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_mdss_dcp_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 1,
	.port_offsets = { 0x39000 },
};

static struct qcom_icc_node qnm_mdss_dcp = {
	.name = "qnm_mdss_dcp",
	.id = CANOE_MASTER_MDSS_DCP,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_mdss_dcp_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static struct qcom_icc_node qnm_vapss_hcp = {
	.name = "qnm_vapss_hcp",
	.id = CANOE_MASTER_CDSP_HCP,
	.channels = 1,
	.buswidth = 32,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_video_cv_cpu_qos = {
	.prio = 4,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x34000 },
};

static struct qcom_icc_node qnm_video_cv_cpu = {
	.name = "qnm_video_cv_cpu",
	.id = CANOE_MASTER_VIDEO_CV_PROC,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_video_cv_cpu_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_video_eva_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x35000, 0x36000 },
};

static struct qcom_icc_node qnm_video_eva = {
	.name = "qnm_video_eva",
	.id = CANOE_MASTER_VIDEO_EVA,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_video_eva_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_video_mvp_qos = {
	.prio = 0,
	.urg_fwd = 1,
	.prio_fwd_disable = 0,
	.num_ports = 2,
	.port_offsets = { 0x32000, 0x33000 },
};

static struct qcom_icc_node qnm_video_mvp = {
	.name = "qnm_video_mvp",
	.id = CANOE_MASTER_VIDEO_MVP,
	.channels = 2,
	.buswidth = 32,
	.qosbox = &qnm_video_mvp_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static const struct qcom_icc_qosbox qnm_video_v_cpu_qos = {
	.prio = 4,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x37000 },
};

static struct qcom_icc_node qnm_video_v_cpu = {
	.name = "qnm_video_v_cpu",
	.id = CANOE_MASTER_VIDEO_V_PROC,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_video_v_cpu_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_MNOC_SF_MEM_NOC },
};

static struct qcom_icc_node qsm_mnoc_cfg = {
	.name = "qsm_mnoc_cfg",
	.id = CANOE_MASTER_CNOC_MNOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_SERVICE_MNOC },
};

static struct qcom_icc_node qnm_nsp = {
	.name = "qnm_nsp",
	.id = CANOE_MASTER_CDSP_PROC,
	.channels = 2,
	.buswidth = 32,
	.num_links = 1,
	.links = { CANOE_SLAVE_CDSP_MEM_NOC },
};

static struct qcom_icc_node qsm_pcie_anoc_cfg = {
	.name = "qsm_pcie_anoc_cfg",
	.id = CANOE_MASTER_PCIE_ANOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_SLAVE_SERVICE_PCIE_ANOC },
};

static const struct qcom_icc_qosbox xm_pcie_qos = {
	.prio = 3,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0xb000 },
};

static struct qcom_icc_node xm_pcie = {
	.name = "xm_pcie",
	.id = CANOE_MASTER_PCIE_0,
	.channels = 1,
	.buswidth = 16,
	.qosbox = &xm_pcie_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_ANOC_PCIE_GEM_NOC },
};

static struct qcom_icc_node qnm_aggre1_noc = {
	.name = "qnm_aggre1_noc",
	.id = CANOE_MASTER_A1NOC_SNOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_SLAVE_SNOC_GEM_NOC_SF },
};

static struct qcom_icc_node qnm_aggre2_noc = {
	.name = "qnm_aggre2_noc",
	.id = CANOE_MASTER_A2NOC_SNOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_SLAVE_SNOC_GEM_NOC_SF },
};

static const struct qcom_icc_qosbox qnm_apss_noc_qos = {
	.prio = 2,
	.urg_fwd = 1,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x1e000 },
};

static struct qcom_icc_node qnm_apss_noc = {
	.name = "qnm_apss_noc",
	.id = CANOE_MASTER_APSS_NOC,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_apss_noc_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_SNOC_GEM_NOC_SF },
};

static const struct qcom_icc_qosbox qnm_cnoc_data_qos = {
	.prio = 2,
	.urg_fwd = 0,
	.prio_fwd_disable = 1,
	.num_ports = 1,
	.port_offsets = { 0x1f000 },
};

static struct qcom_icc_node qnm_cnoc_data = {
	.name = "qnm_cnoc_data",
	.id = CANOE_MASTER_CNOC_SNOC,
	.channels = 1,
	.buswidth = 8,
	.qosbox = &qnm_cnoc_data_qos,
	.num_links = 1,
	.links = { CANOE_SLAVE_SNOC_GEM_NOC_SF },
};

static struct qcom_icc_node qns_a1noc_snoc = {
	.name = "qns_a1noc_snoc",
	.id = CANOE_SLAVE_A1NOC_SNOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_A1NOC_SNOC },
};

static struct qcom_icc_node qns_a2noc_snoc = {
	.name = "qns_a2noc_snoc",
	.id = CANOE_SLAVE_A2NOC_SNOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_A2NOC_SNOC },
};

static struct qcom_icc_node qup0_core_slave = {
	.name = "qup0_core_slave",
	.id = CANOE_SLAVE_QUP_CORE_0,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qup1_core_slave = {
	.name = "qup1_core_slave",
	.id = CANOE_SLAVE_QUP_CORE_1,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qup2_core_slave = {
	.name = "qup2_core_slave",
	.id = CANOE_SLAVE_QUP_CORE_2,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qup3_core_slave = {
	.name = "qup3_core_slave",
	.id = CANOE_SLAVE_QUP_CORE_3,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qup4_core_slave = {
	.name = "qup4_core_slave",
	.id = CANOE_SLAVE_QUP_CORE_4,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ahb2phy0 = {
	.name = "qhs_ahb2phy0",
	.id = CANOE_SLAVE_AHB2PHY_SOUTH,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ahb2phy1 = {
	.name = "qhs_ahb2phy1",
	.id = CANOE_SLAVE_AHB2PHY_NORTH,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_camera_cfg = {
	.name = "qhs_camera_cfg",
	.id = CANOE_SLAVE_CAMERA_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_clk_ctl = {
	.name = "qhs_clk_ctl",
	.id = CANOE_SLAVE_CLK_CTL,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_crypto0_cfg = {
	.name = "qhs_crypto0_cfg",
	.id = CANOE_SLAVE_CRYPTO_0_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_display_cfg = {
	.name = "qhs_display_cfg",
	.id = CANOE_SLAVE_DISPLAY_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_eva_cfg = {
	.name = "qhs_eva_cfg",
	.id = CANOE_SLAVE_EVA_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_gpuss_cfg = {
	.name = "qhs_gpuss_cfg",
	.id = CANOE_SLAVE_GFX3D_CFG,
	.channels = 1,
	.buswidth = 8,
	.num_links = 0,
};

static struct qcom_icc_node qhs_i2c = {
	.name = "qhs_i2c",
	.id = CANOE_SLAVE_I2C,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_i3c_ibi0_cfg = {
	.name = "qhs_i3c_ibi0_cfg",
	.id = CANOE_SLAVE_I3C_IBI0_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_i3c_ibi1_cfg = {
	.name = "qhs_i3c_ibi1_cfg",
	.id = CANOE_SLAVE_I3C_IBI1_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_imem_cfg = {
	.name = "qhs_imem_cfg",
	.id = CANOE_SLAVE_IMEM_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ipc_router = {
	.name = "qhs_ipc_router",
	.id = CANOE_SLAVE_IPC_ROUTER_CFG,
	.channels = 4,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_mss_cfg = {
	.name = "qhs_mss_cfg",
	.id = CANOE_SLAVE_CNOC_MSS,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_pcie_cfg = {
	.name = "qhs_pcie_cfg",
	.id = CANOE_SLAVE_PCIE_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_prng = {
	.name = "qhs_prng",
	.id = CANOE_SLAVE_PRNG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qdss_cfg = {
	.name = "qhs_qdss_cfg",
	.id = CANOE_SLAVE_QDSS_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qspi = {
	.name = "qhs_qspi",
	.id = CANOE_SLAVE_QSPI_0,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qup1 = {
	.name = "qhs_qup1",
	.id = CANOE_SLAVE_QUP_1,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qup2 = {
	.name = "qhs_qup2",
	.id = CANOE_SLAVE_QUP_2,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qup3 = {
	.name = "qhs_qup3",
	.id = CANOE_SLAVE_QUP_3,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_qup4 = {
	.name = "qhs_qup4",
	.id = CANOE_SLAVE_QUP_4,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_sdc2 = {
	.name = "qhs_sdc2",
	.id = CANOE_SLAVE_SDCC_2,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_sdc4 = {
	.name = "qhs_sdc4",
	.id = CANOE_SLAVE_SDCC_4,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_spss_cfg = {
	.name = "qhs_spss_cfg",
	.id = CANOE_SLAVE_SPSS_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_tcsr = {
	.name = "qhs_tcsr",
	.id = CANOE_SLAVE_TCSR,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_tlmm = {
	.name = "qhs_tlmm",
	.id = CANOE_SLAVE_TLMM,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ufs_mem_cfg = {
	.name = "qhs_ufs_mem_cfg",
	.id = CANOE_SLAVE_UFS_MEM_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_usb3 = {
	.name = "qhs_usb3",
	.id = CANOE_SLAVE_USB3,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_venus_cfg = {
	.name = "qhs_venus_cfg",
	.id = CANOE_SLAVE_VENUS_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_vsense_ctrl_cfg = {
	.name = "qhs_vsense_ctrl_cfg",
	.id = CANOE_SLAVE_VSENSE_CTRL_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qss_mnoc_cfg = {
	.name = "qss_mnoc_cfg",
	.id = CANOE_SLAVE_CNOC_MNOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_MASTER_CNOC_MNOC_CFG },
};

static struct qcom_icc_node qss_pcie_anoc_cfg = {
	.name = "qss_pcie_anoc_cfg",
	.id = CANOE_SLAVE_PCIE_ANOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_MASTER_PCIE_ANOC_CFG },
};

static struct qcom_icc_node xs_qdss_stm = {
	.name = "xs_qdss_stm",
	.id = CANOE_SLAVE_QDSS_STM,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node xs_sys_tcu_cfg = {
	.name = "xs_sys_tcu_cfg",
	.id = CANOE_SLAVE_TCU,
	.channels = 1,
	.buswidth = 8,
	.num_links = 0,
};

static struct qcom_icc_node qhs_aoss = {
	.name = "qhs_aoss",
	.id = CANOE_SLAVE_AOSS,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ipa = {
	.name = "qhs_ipa",
	.id = CANOE_SLAVE_IPA_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_ipc_router_fence = {
	.name = "qhs_ipc_router_fence",
	.id = CANOE_SLAVE_IPC_ROUTER_FENCE,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_soccp = {
	.name = "qhs_soccp",
	.id = CANOE_SLAVE_SOCCP,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qhs_tme_cfg = {
	.name = "qhs_tme_cfg",
	.id = CANOE_SLAVE_TME_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qns_apss = {
	.name = "qns_apss",
	.id = CANOE_SLAVE_APPSS,
	.channels = 1,
	.buswidth = 8,
	.num_links = 0,
};

static struct qcom_icc_node qss_cfg = {
	.name = "qss_cfg",
	.id = CANOE_SLAVE_CNOC_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 1,
	.links = { CANOE_MASTER_CNOC_CFG },
};

static struct qcom_icc_node qss_ddrss_cfg = {
	.name = "qss_ddrss_cfg",
	.id = CANOE_SLAVE_DDRSS_CFG,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qxs_boot_imem = {
	.name = "qxs_boot_imem",
	.id = CANOE_SLAVE_BOOT_IMEM,
	.channels = 1,
	.buswidth = 16,
	.num_links = 0,
};

static struct qcom_icc_node qxs_imem = {
	.name = "qxs_imem",
	.id = CANOE_SLAVE_IMEM,
	.channels = 1,
	.buswidth = 8,
	.num_links = 0,
};

static struct qcom_icc_node xs_pcie = {
	.name = "xs_pcie",
	.id = CANOE_SLAVE_PCIE_0,
	.channels = 1,
	.buswidth = 16,
	.num_links = 0,
};

static struct qcom_icc_node qns_gem_noc_cnoc = {
	.name = "qns_gem_noc_cnoc",
	.id = CANOE_SLAVE_GEM_NOC_CNOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_GEM_NOC_CNOC },
};

static struct qcom_icc_node qns_llcc = {
	.name = "qns_llcc",
	.id = CANOE_SLAVE_LLCC,
	.channels = 4,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_LLCC },
};

static struct qcom_icc_node qns_pcie = {
	.name = "qns_pcie",
	.id = CANOE_SLAVE_MEM_NOC_PCIE_SNOC,
	.channels = 1,
	.buswidth = 8,
	.num_links = 1,
	.links = { CANOE_MASTER_GEM_NOC_PCIE_SNOC },
};

static struct qcom_icc_node qns_lpass_ag_noc_gemnoc = {
	.name = "qns_lpass_ag_noc_gemnoc",
	.id = CANOE_SLAVE_LPASS_GEM_NOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_LPASS_GEM_NOC },
};

static struct qcom_icc_node qns_lpass_aggnoc = {
	.name = "qns_lpass_aggnoc",
	.id = CANOE_SLAVE_LPIAON_NOC_LPASS_AG_NOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_LPIAON_NOC },
};

static struct qcom_icc_node qns_lpi_aon_noc = {
	.name = "qns_lpi_aon_noc",
	.id = CANOE_SLAVE_LPICX_NOC_LPIAON_NOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_LPASS_LPINOC },
};

static struct qcom_icc_node ebi = {
	.name = "ebi",
	.id = CANOE_SLAVE_EBI1,
	.channels = 4,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qns_mem_noc_hf = {
	.name = "qns_mem_noc_hf",
	.id = CANOE_SLAVE_MNOC_HF_MEM_NOC,
	.channels = 2,
	.buswidth = 32,
	.num_links = 1,
	.links = { CANOE_MASTER_MNOC_HF_MEM_NOC },
};

static struct qcom_icc_node qns_mem_noc_sf = {
	.name = "qns_mem_noc_sf",
	.id = CANOE_SLAVE_MNOC_SF_MEM_NOC,
	.channels = 2,
	.buswidth = 32,
	.num_links = 1,
	.links = { CANOE_MASTER_MNOC_SF_MEM_NOC },
};

static struct qcom_icc_node srvc_mnoc = {
	.name = "srvc_mnoc",
	.id = CANOE_SLAVE_SERVICE_MNOC,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qns_nsp_gemnoc = {
	.name = "qns_nsp_gemnoc",
	.id = CANOE_SLAVE_CDSP_MEM_NOC,
	.channels = 2,
	.buswidth = 32,
	.num_links = 1,
	.links = { CANOE_MASTER_COMPUTE_NOC },
};

static struct qcom_icc_node qns_pcie_gemnoc = {
	.name = "qns_pcie_gemnoc",
	.id = CANOE_SLAVE_ANOC_PCIE_GEM_NOC,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_ANOC_PCIE_GEM_NOC },
};

static struct qcom_icc_node srvc_pcie_aggre_noc = {
	.name = "srvc_pcie_aggre_noc",
	.id = CANOE_SLAVE_SERVICE_PCIE_ANOC,
	.channels = 1,
	.buswidth = 4,
	.num_links = 0,
};

static struct qcom_icc_node qns_gemnoc_sf = {
	.name = "qns_gemnoc_sf",
	.id = CANOE_SLAVE_SNOC_GEM_NOC_SF,
	.channels = 1,
	.buswidth = 16,
	.num_links = 1,
	.links = { CANOE_MASTER_SNOC_SF_MEM_NOC },
};

static struct qcom_icc_bcm bcm_acv = {
	.name = "ACV",
	.num_nodes = 1,
	.nodes = { &ebi },
};

static struct qcom_icc_bcm bcm_ce0 = {
	.name = "CE0",
	.num_nodes = 1,
	.nodes = { &qxm_crypto },
};

static struct qcom_icc_bcm bcm_cn0 = {
	.name = "CN0",
	.enable_mask = 0x1,
	.keepalive = true,
	.num_nodes = 43,
	.nodes = { &qsm_cfg, &qhs_ahb2phy0,
		   &qhs_ahb2phy1, &qhs_camera_cfg,
		   &qhs_clk_ctl, &qhs_crypto0_cfg,
		   &qhs_eva_cfg, &qhs_gpuss_cfg,
		   &qhs_i3c_ibi0_cfg, &qhs_i3c_ibi1_cfg,
		   &qhs_imem_cfg, &qhs_ipc_router,
		   &qhs_mss_cfg, &qhs_pcie_cfg,
		   &qhs_prng, &qhs_qdss_cfg,
		   &qhs_qspi, &qhs_sdc2,
		   &qhs_sdc4, &qhs_spss_cfg,
		   &qhs_tcsr, &qhs_tlmm,
		   &qhs_ufs_mem_cfg, &qhs_usb3,
		   &qhs_venus_cfg, &qhs_vsense_ctrl_cfg,
		   &qss_mnoc_cfg, &qss_pcie_anoc_cfg,
		   &xs_qdss_stm, &xs_sys_tcu_cfg,
		   &qnm_gemnoc_cnoc, &qnm_gemnoc_pcie,
		   &qhs_aoss, &qhs_ipa,
		   &qhs_ipc_router_fence, &qhs_soccp,
		   &qhs_tme_cfg, &qns_apss,
		   &qss_cfg, &qss_ddrss_cfg,
		   &qxs_boot_imem, &qxs_imem,
		   &xs_pcie },
};

static struct qcom_icc_bcm bcm_cn1 = {
	.name = "CN1",
	.num_nodes = 6,
	.nodes = { &qhs_display_cfg, &qhs_i2c,
		   &qhs_qup1, &qhs_qup2,
		   &qhs_qup3, &qhs_qup4 },
};

static struct qcom_icc_bcm bcm_co0 = {
	.name = "CO0",
	.enable_mask = 0x1,
	.num_nodes = 2,
	.nodes = { &qnm_nsp, &qns_nsp_gemnoc },
};

static struct qcom_icc_bcm bcm_lp0 = {
	.name = "LP0",
	.num_nodes = 2,
	.nodes = { &qnm_lpass_lpinoc, &qns_lpass_aggnoc },
};

static struct qcom_icc_bcm bcm_mc0 = {
	.name = "MC0",
	.keepalive = true,
	.num_nodes = 1,
	.nodes = { &ebi },
};

static struct qcom_icc_bcm bcm_mm0 = {
	.name = "MM0",
	.num_nodes = 1,
	.nodes = { &qns_mem_noc_hf },
};

static struct qcom_icc_bcm bcm_mm1 = {
	.name = "MM1",
	.enable_mask = 0x1,
	.num_nodes = 9,
	.nodes = { &qnm_camnoc_hf, &qnm_camnoc_nrt_icp_sf,
		   &qnm_camnoc_rt_cdm_sf, &qnm_camnoc_sf,
		   &qnm_vapss_hcp, &qnm_video_cv_cpu,
		   &qnm_video_mvp, &qnm_video_v_cpu,
		   &qns_mem_noc_sf },
};

static struct qcom_icc_bcm bcm_qpc0 = {
	.name = "QPC0",
	.num_nodes = 1,
	.nodes = { &qnm_qpace },
};

static struct qcom_icc_bcm bcm_qup0 = {
	.name = "QUP0",
	.vote_scale = 1,
	.num_nodes = 1,
	.nodes = { &qup0_core_slave },
};

static struct qcom_icc_bcm bcm_qup1 = {
	.name = "QUP1",
	.vote_scale = 1,
	.num_nodes = 1,
	.nodes = { &qup1_core_slave },
};

static struct qcom_icc_bcm bcm_qup2 = {
	.name = "QUP2",
	.vote_scale = 1,
	.num_nodes = 1,
	.nodes = { &qup2_core_slave },
};

static struct qcom_icc_bcm bcm_qup3 = {
	.name = "QUP3",
	.vote_scale = 1,
	.num_nodes = 1,
	.nodes = { &qup3_core_slave },
};

static struct qcom_icc_bcm bcm_qup4 = {
	.name = "QUP4",
	.vote_scale = 1,
	.num_nodes = 1,
	.nodes = { &qup4_core_slave },
};

static struct qcom_icc_bcm bcm_sh0 = {
	.name = "SH0",
	.keepalive = true,
	.num_nodes = 1,
	.nodes = { &qns_llcc },
};

static struct qcom_icc_bcm bcm_sh1 = {
	.name = "SH1",
	.enable_mask = 0x1,
	.num_nodes = 14,
	.nodes = { &alm_gpu_tcu, &alm_sys_tcu,
		   &chm_apps, &qnm_gpu,
		   &qnm_mdsp, &qnm_mnoc_hf,
		   &qnm_mnoc_sf, &qnm_nsp_gemnoc,
		   &qnm_pcie, &qnm_snoc_sf,
		   &qnm_wlan_q6, &xm_gic,
		   &qns_gem_noc_cnoc, &qns_pcie },
};

static struct qcom_icc_bcm bcm_sn0 = {
	.name = "SN0",
	.keepalive = true,
	.num_nodes = 1,
	.nodes = { &qns_gemnoc_sf },
};

static struct qcom_icc_bcm bcm_sn2 = {
	.name = "SN2",
	.num_nodes = 1,
	.nodes = { &qnm_aggre1_noc },
};

static struct qcom_icc_bcm bcm_sn3 = {
	.name = "SN3",
	.num_nodes = 1,
	.nodes = { &qnm_aggre2_noc },
};

static struct qcom_icc_bcm bcm_sn4 = {
	.name = "SN4",
	.num_nodes = 1,
	.nodes = { &qns_pcie_gemnoc },
};

static struct qcom_icc_bcm * const aggre_noc_bcms[] = {
	&bcm_ce0,
};

static struct qcom_icc_node * const aggre_noc_nodes[] = {
	[CANOE_MASTER_QSPI_0] = &qhm_qspi,
	[CANOE_MASTER_CRYPTO] = &qxm_crypto,
	[CANOE_MASTER_QUP_1] = &qxm_qup1,
	[CANOE_MASTER_SDCC_4] = &xm_sdc4,
	[CANOE_MASTER_UFS_MEM] = &xm_ufs_mem,
	[CANOE_MASTER_USB3] = &xm_usb3,
	[CANOE_SLAVE_A1NOC_SNOC] = &qns_a1noc_snoc,
	[CANOE_MASTER_QUP_2] = &qhm_qup2,
	[CANOE_MASTER_QUP_3] = &qhm_qup3,
	[CANOE_MASTER_QUP_4] = &qhm_qup4,
	[CANOE_MASTER_IPA] = &qxm_ipa,
	[CANOE_MASTER_SOCCP_AGGR_NOC] = &qxm_soccp,
	[CANOE_MASTER_SP] = &qxm_sp,
	[CANOE_MASTER_QDSS_ETR] = &xm_qdss_etr_0,
	[CANOE_MASTER_QDSS_ETR_1] = &xm_qdss_etr_1,
	[CANOE_MASTER_SDCC_2] = &xm_sdc2,
	[CANOE_SLAVE_A2NOC_SNOC] = &qns_a2noc_snoc,
};

static const struct qcom_icc_desc canoe_aggre_noc = {
	.config = &icc_regmap_config,
	.nodes = aggre_noc_nodes,
	.num_nodes = ARRAY_SIZE(aggre_noc_nodes),
	.bcms = aggre_noc_bcms,
	.num_bcms = ARRAY_SIZE(aggre_noc_bcms),
};

static struct qcom_icc_bcm * const clk_virt_bcms[] = {
	&bcm_qup0,
	&bcm_qup1,
	&bcm_qup2,
	&bcm_qup3,
	&bcm_qup4,

};

static struct qcom_icc_node * const clk_virt_nodes[] = {
	[CANOE_MASTER_QUP_CORE_0] = &qup0_core_master,
	[CANOE_MASTER_QUP_CORE_1] = &qup1_core_master,
	[CANOE_MASTER_QUP_CORE_2] = &qup2_core_master,
	[CANOE_MASTER_QUP_CORE_3] = &qup3_core_master,
	[CANOE_MASTER_QUP_CORE_4] = &qup4_core_master,
	[CANOE_SLAVE_QUP_CORE_0] = &qup0_core_slave,
	[CANOE_SLAVE_QUP_CORE_1] = &qup1_core_slave,
	[CANOE_SLAVE_QUP_CORE_2] = &qup2_core_slave,
	[CANOE_SLAVE_QUP_CORE_3] = &qup3_core_slave,
	[CANOE_SLAVE_QUP_CORE_4] = &qup4_core_slave,

};

static const struct qcom_icc_desc canoe_clk_virt = {
	.config = &icc_regmap_config,
	.nodes = clk_virt_nodes,
	.num_nodes = ARRAY_SIZE(clk_virt_nodes),
	.bcms = clk_virt_bcms,
	.num_bcms = ARRAY_SIZE(clk_virt_bcms),
};

static struct qcom_icc_bcm * const cnoc_cfg_bcms[] = {
	&bcm_cn0,
	&bcm_cn1,
};

static struct qcom_icc_node * const cnoc_cfg_nodes[] = {
	[CANOE_MASTER_CNOC_CFG] = &qsm_cfg,
	[CANOE_SLAVE_AHB2PHY_SOUTH] = &qhs_ahb2phy0,
	[CANOE_SLAVE_AHB2PHY_NORTH] = &qhs_ahb2phy1,
	[CANOE_SLAVE_CAMERA_CFG] = &qhs_camera_cfg,
	[CANOE_SLAVE_CLK_CTL] = &qhs_clk_ctl,
	[CANOE_SLAVE_CRYPTO_0_CFG] = &qhs_crypto0_cfg,
	[CANOE_SLAVE_DISPLAY_CFG] = &qhs_display_cfg,
	[CANOE_SLAVE_EVA_CFG] = &qhs_eva_cfg,
	[CANOE_SLAVE_GFX3D_CFG] = &qhs_gpuss_cfg,
	[CANOE_SLAVE_I2C] = &qhs_i2c,
	[CANOE_SLAVE_I3C_IBI0_CFG] = &qhs_i3c_ibi0_cfg,
	[CANOE_SLAVE_I3C_IBI1_CFG] = &qhs_i3c_ibi1_cfg,
	[CANOE_SLAVE_IMEM_CFG] = &qhs_imem_cfg,
	[CANOE_SLAVE_IPC_ROUTER_CFG] = &qhs_ipc_router,
	[CANOE_SLAVE_CNOC_MSS] = &qhs_mss_cfg,
	[CANOE_SLAVE_PCIE_CFG] = &qhs_pcie_cfg,
	[CANOE_SLAVE_PRNG] = &qhs_prng,
	[CANOE_SLAVE_QDSS_CFG] = &qhs_qdss_cfg,
	[CANOE_SLAVE_QSPI_0] = &qhs_qspi,
	[CANOE_SLAVE_QUP_1] = &qhs_qup1,
	[CANOE_SLAVE_QUP_2] = &qhs_qup2,
	[CANOE_SLAVE_QUP_3] = &qhs_qup3,
	[CANOE_SLAVE_QUP_4] = &qhs_qup4,
	[CANOE_SLAVE_SDCC_2] = &qhs_sdc2,
	[CANOE_SLAVE_SDCC_4] = &qhs_sdc4,
	[CANOE_SLAVE_SPSS_CFG] = &qhs_spss_cfg,
	[CANOE_SLAVE_TCSR] = &qhs_tcsr,
	[CANOE_SLAVE_TLMM] = &qhs_tlmm,
	[CANOE_SLAVE_UFS_MEM_CFG] = &qhs_ufs_mem_cfg,
	[CANOE_SLAVE_USB3] = &qhs_usb3,
	[CANOE_SLAVE_VENUS_CFG] = &qhs_venus_cfg,
	[CANOE_SLAVE_VSENSE_CTRL_CFG] = &qhs_vsense_ctrl_cfg,
	[CANOE_SLAVE_CNOC_MNOC_CFG] = &qss_mnoc_cfg,
	[CANOE_SLAVE_PCIE_ANOC_CFG] = &qss_pcie_anoc_cfg,
	[CANOE_SLAVE_QDSS_STM] = &xs_qdss_stm,
	[CANOE_SLAVE_TCU] = &xs_sys_tcu_cfg,
};

static const struct qcom_icc_desc canoe_cnoc_cfg = {
	.config = &icc_regmap_config,
	.nodes = cnoc_cfg_nodes,
	.num_nodes = ARRAY_SIZE(cnoc_cfg_nodes),
	.bcms = cnoc_cfg_bcms,
	.num_bcms = ARRAY_SIZE(cnoc_cfg_bcms),
};

static struct qcom_icc_bcm * const cnoc_main_bcms[] = {
	&bcm_cn0,
};

static struct qcom_icc_node * const cnoc_main_nodes[] = {
	[CANOE_MASTER_GEM_NOC_CNOC] = &qnm_gemnoc_cnoc,
	[CANOE_MASTER_GEM_NOC_PCIE_SNOC] = &qnm_gemnoc_pcie,
	[CANOE_SLAVE_AOSS] = &qhs_aoss,
	[CANOE_SLAVE_IPA_CFG] = &qhs_ipa,
	[CANOE_SLAVE_IPC_ROUTER_FENCE] = &qhs_ipc_router_fence,
	[CANOE_SLAVE_SOCCP] = &qhs_soccp,
	[CANOE_SLAVE_TME_CFG] = &qhs_tme_cfg,
	[CANOE_SLAVE_APPSS] = &qns_apss,
	[CANOE_SLAVE_CNOC_CFG] = &qss_cfg,
	[CANOE_SLAVE_DDRSS_CFG] = &qss_ddrss_cfg,
	[CANOE_SLAVE_BOOT_IMEM] = &qxs_boot_imem,
	[CANOE_SLAVE_IMEM] = &qxs_imem,
	[CANOE_SLAVE_PCIE_0] = &xs_pcie,
};

static const struct qcom_icc_desc canoe_cnoc_main = {
	.config = &icc_regmap_config,
	.nodes = cnoc_main_nodes,
	.num_nodes = ARRAY_SIZE(cnoc_main_nodes),
	.bcms = cnoc_main_bcms,
	.num_bcms = ARRAY_SIZE(cnoc_main_bcms),
};

static struct qcom_icc_bcm * const gem_noc_bcms[] = {
	&bcm_qpc0,
	&bcm_sh0,
	&bcm_sh1,

};

static struct qcom_icc_node * const gem_noc_nodes[] = {
	[CANOE_MASTER_GPU_TCU] = &alm_gpu_tcu,
	[CANOE_MASTER_SYS_TCU] = &alm_sys_tcu,
	[CANOE_MASTER_APPSS_PROC] = &chm_apps,
	[CANOE_MASTER_GFX3D] = &qnm_gpu,
	[CANOE_MASTER_LPASS_GEM_NOC] = &qnm_lpass_gemnoc,
	[CANOE_MASTER_MSS_PROC] = &qnm_mdsp,
	[CANOE_MASTER_MNOC_HF_MEM_NOC] = &qnm_mnoc_hf,
	[CANOE_MASTER_MNOC_SF_MEM_NOC] = &qnm_mnoc_sf,
	[CANOE_MASTER_COMPUTE_NOC] = &qnm_nsp_gemnoc,
	[CANOE_MASTER_ANOC_PCIE_GEM_NOC] = &qnm_pcie,
	[CANOE_MASTER_QPACE] = &qnm_qpace,
	[CANOE_MASTER_SNOC_SF_MEM_NOC] = &qnm_snoc_sf,
	[CANOE_MASTER_WLAN_Q6] = &qnm_wlan_q6,
	[CANOE_MASTER_GIC] = &xm_gic,
	[CANOE_SLAVE_GEM_NOC_CNOC] = &qns_gem_noc_cnoc,
	[CANOE_SLAVE_LLCC] = &qns_llcc,
	[CANOE_SLAVE_MEM_NOC_PCIE_SNOC] = &qns_pcie,

};

static const struct qcom_icc_desc canoe_gem_noc = {
	.config = &icc_regmap_config,
	.nodes = gem_noc_nodes,
	.num_nodes = ARRAY_SIZE(gem_noc_nodes),
	.bcms = gem_noc_bcms,
	.num_bcms = ARRAY_SIZE(gem_noc_bcms),
};

static struct qcom_icc_bcm * const lpass_ag_noc_bcms[] = {
};

static struct qcom_icc_node * const lpass_ag_noc_nodes[] = {
	[CANOE_MASTER_LPIAON_NOC] = &qnm_lpiaon_noc,
	[CANOE_SLAVE_LPASS_GEM_NOC] = &qns_lpass_ag_noc_gemnoc,
};

static const struct qcom_icc_desc canoe_lpass_ag_noc = {
	.config = &icc_regmap_config,
	.nodes = lpass_ag_noc_nodes,
	.num_nodes = ARRAY_SIZE(lpass_ag_noc_nodes),
	.bcms = lpass_ag_noc_bcms,
	.num_bcms = ARRAY_SIZE(lpass_ag_noc_bcms),
};

static struct qcom_icc_bcm * const lpass_lpiaon_noc_bcms[] = {
	&bcm_lp0,
};

static struct qcom_icc_node * const lpass_lpiaon_noc_nodes[] = {
	[CANOE_MASTER_LPASS_LPINOC] = &qnm_lpass_lpinoc,
	[CANOE_SLAVE_LPIAON_NOC_LPASS_AG_NOC] = &qns_lpass_aggnoc,
};

static const struct qcom_icc_desc canoe_lpass_lpiaon_noc = {
	.config = &icc_regmap_config,
	.nodes = lpass_lpiaon_noc_nodes,
	.num_nodes = ARRAY_SIZE(lpass_lpiaon_noc_nodes),
	.bcms = lpass_lpiaon_noc_bcms,
	.num_bcms = ARRAY_SIZE(lpass_lpiaon_noc_bcms),
};

static struct qcom_icc_bcm * const lpass_lpicx_noc_bcms[] = {
};

static struct qcom_icc_node * const lpass_lpicx_noc_nodes[] = {
	[CANOE_MASTER_LPASS_PROC] = &qnm_lpinoc_dsp_qns4m,
	[CANOE_SLAVE_LPICX_NOC_LPIAON_NOC] = &qns_lpi_aon_noc,
};

static const struct qcom_icc_desc canoe_lpass_lpicx_noc = {
	.config = &icc_regmap_config,
	.nodes = lpass_lpicx_noc_nodes,
	.num_nodes = ARRAY_SIZE(lpass_lpicx_noc_nodes),
	.bcms = lpass_lpicx_noc_bcms,
	.num_bcms = ARRAY_SIZE(lpass_lpicx_noc_bcms),
};

static struct qcom_icc_bcm * const mc_virt_bcms[] = {
	&bcm_acv,
	&bcm_mc0,

};

static struct qcom_icc_node * const mc_virt_nodes[] = {
	[CANOE_MASTER_LLCC] = &llcc_mc,
	[CANOE_SLAVE_EBI1] = &ebi,

};

static const struct qcom_icc_desc canoe_mc_virt = {
	.config = &icc_regmap_config,
	.nodes = mc_virt_nodes,
	.num_nodes = ARRAY_SIZE(mc_virt_nodes),
	.bcms = mc_virt_bcms,
	.num_bcms = ARRAY_SIZE(mc_virt_bcms),
};

static struct qcom_icc_bcm * const mmss_noc_bcms[] = {
	&bcm_mm0,
	&bcm_mm1,

};

static struct qcom_icc_node * const mmss_noc_nodes[] = {
	[CANOE_MASTER_CAMNOC_HF] = &qnm_camnoc_hf,
	[CANOE_MASTER_CAMNOC_NRT_ICP_SF] = &qnm_camnoc_nrt_icp_sf,
	[CANOE_MASTER_CAMNOC_RT_CDM_SF] = &qnm_camnoc_rt_cdm_sf,
	[CANOE_MASTER_CAMNOC_SF] = &qnm_camnoc_sf,
	[CANOE_MASTER_MDP] = &qnm_mdp,
	[CANOE_MASTER_MDSS_DCP] = &qnm_mdss_dcp,
	[CANOE_MASTER_CDSP_HCP] = &qnm_vapss_hcp,
	[CANOE_MASTER_VIDEO_CV_PROC] = &qnm_video_cv_cpu,
	[CANOE_MASTER_VIDEO_EVA] = &qnm_video_eva,
	[CANOE_MASTER_VIDEO_MVP] = &qnm_video_mvp,
	[CANOE_MASTER_VIDEO_V_PROC] = &qnm_video_v_cpu,
	[CANOE_MASTER_CNOC_MNOC_CFG] = &qsm_mnoc_cfg,
	[CANOE_SLAVE_MNOC_HF_MEM_NOC] = &qns_mem_noc_hf,
	[CANOE_SLAVE_MNOC_SF_MEM_NOC] = &qns_mem_noc_sf,
	[CANOE_SLAVE_SERVICE_MNOC] = &srvc_mnoc,

};

static const struct qcom_icc_desc canoe_mmss_noc = {
	.config = &icc_regmap_config,
	.nodes = mmss_noc_nodes,
	.num_nodes = ARRAY_SIZE(mmss_noc_nodes),
	.bcms = mmss_noc_bcms,
	.num_bcms = ARRAY_SIZE(mmss_noc_bcms),
};

static struct qcom_icc_bcm * const nsp_noc_bcms[] = {
	&bcm_co0,
};

static struct qcom_icc_node * const nsp_noc_nodes[] = {
	[CANOE_MASTER_CDSP_PROC] = &qnm_nsp,
	[CANOE_SLAVE_CDSP_MEM_NOC] = &qns_nsp_gemnoc,
};

static const struct qcom_icc_desc canoe_nsp_noc = {
	.config = &icc_regmap_config,
	.nodes = nsp_noc_nodes,
	.num_nodes = ARRAY_SIZE(nsp_noc_nodes),
	.bcms = nsp_noc_bcms,
	.num_bcms = ARRAY_SIZE(nsp_noc_bcms),
};

static struct qcom_icc_bcm * const pcie_anoc_bcms[] = {
	&bcm_sn4,

};

static struct qcom_icc_node * const pcie_anoc_nodes[] = {
	[CANOE_MASTER_PCIE_ANOC_CFG] = &qsm_pcie_anoc_cfg,
	[CANOE_MASTER_PCIE_0] = &xm_pcie,
	[CANOE_SLAVE_ANOC_PCIE_GEM_NOC] = &qns_pcie_gemnoc,
	[CANOE_SLAVE_SERVICE_PCIE_ANOC] = &srvc_pcie_aggre_noc,

};

static const struct qcom_icc_desc canoe_pcie_anoc = {
	.config = &icc_regmap_config,
	.nodes = pcie_anoc_nodes,
	.num_nodes = ARRAY_SIZE(pcie_anoc_nodes),
	.bcms = pcie_anoc_bcms,
	.num_bcms = ARRAY_SIZE(pcie_anoc_bcms),
};

static struct qcom_icc_bcm * const system_noc_bcms[] = {
	&bcm_sn0,
	&bcm_sn2,
	&bcm_sn3,
};

static struct qcom_icc_node * const system_noc_nodes[] = {
	[CANOE_MASTER_A1NOC_SNOC] = &qnm_aggre1_noc,
	[CANOE_MASTER_A2NOC_SNOC] = &qnm_aggre2_noc,
	[CANOE_MASTER_APSS_NOC] = &qnm_apss_noc,
	[CANOE_MASTER_CNOC_SNOC] = &qnm_cnoc_data,
	[CANOE_SLAVE_SNOC_GEM_NOC_SF] = &qns_gemnoc_sf,
};

static const struct qcom_icc_desc canoe_system_noc = {
	.config = &icc_regmap_config,
	.nodes = system_noc_nodes,
	.num_nodes = ARRAY_SIZE(system_noc_nodes),
	.bcms = system_noc_bcms,
	.num_bcms = ARRAY_SIZE(system_noc_bcms),
};

static const struct of_device_id qnoc_of_match[] = {
	{ .compatible = "qcom,canoe-aggre-noc", .data = &canoe_aggre_noc },
	{ .compatible = "qcom,canoe-clk-virt", .data = &canoe_clk_virt },
	{ .compatible = "qcom,canoe-cnoc-cfg", .data = &canoe_cnoc_cfg },
	{ .compatible = "qcom,canoe-cnoc-main", .data = &canoe_cnoc_main },
	{ .compatible = "qcom,canoe-gem-noc", .data = &canoe_gem_noc },
	{ .compatible = "qcom,canoe-lpass-ag-noc", .data = &canoe_lpass_ag_noc },
	{ .compatible = "qcom,canoe-lpass-lpiaon-noc", .data = &canoe_lpass_lpiaon_noc },
	{ .compatible = "qcom,canoe-lpass-lpicx-noc", .data = &canoe_lpass_lpicx_noc },
	{ .compatible = "qcom,canoe-mc-virt", .data = &canoe_mc_virt },
	{ .compatible = "qcom,canoe-mmss-noc", .data = &canoe_mmss_noc },
	{ .compatible = "qcom,canoe-nsp-noc", .data = &canoe_nsp_noc },
	{ .compatible = "qcom,canoe-pcie-anoc", .data = &canoe_pcie_anoc },
	{ .compatible = "qcom,canoe-system-noc", .data = &canoe_system_noc },
	{ }
};
MODULE_DEVICE_TABLE(of, qnoc_of_match);

static struct platform_driver qnoc_driver = {
	.probe = qcom_icc_rpmh_probe,
	.remove_new = qcom_icc_rpmh_remove,
	.driver = {
		.name = "qnoc-canoe",
		.of_match_table = qnoc_of_match,
		.sync_state = icc_sync_state,
	},
};

static int __init qnoc_driver_init(void)
{
	return platform_driver_register(&qnoc_driver);
}
core_initcall(qnoc_driver_init);

static void __exit qnoc_driver_exit(void)
{
	platform_driver_unregister(&qnoc_driver);
}
module_exit(qnoc_driver_exit);

MODULE_DESCRIPTION("Canoe NoC driver");
MODULE_LICENSE("GPL");
