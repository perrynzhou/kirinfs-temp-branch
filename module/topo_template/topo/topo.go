package topo

import (
	"encoding/json"
	"fmt"
	"math/rand"
	"os"
	"topo_template/utils"
)

const (
	defaultNodeCnt    = 4
	defaultNodeStatus = "active"
	defaultNodePort   = 6335
)

type FsTopo struct {
	FsName string                `json:"fs_name"`
	Role   int                   `json:"node_role"`
	Nodes  map[int][]*utils.Node `json:"node_topo"`
}

func NewBatchNode(cnt, port, nodeType int, ipAddrPrefix, nodeStatus string) ([]*utils.Node, error) {
	nodes := make([]*utils.Node, cnt)
	var err error
	for i := 0; i < cnt; i++ {
		nodes[i], err = utils.NewNode(nodeType, fmt.Sprintf("%s.%d", ipAddrPrefix, i), port, nodeStatus)
		if err != nil {
			return nil, err
		}
	}
	return nodes, nil
}
func NewTopo(role int, fsName string) (*FsTopo, error) {
	var err error
	nodeCnt := rand.Intn(defaultNodeCnt) + 1
	nodes := make(map[int][]*utils.Node)
	if nodes[utils.MgmtNodeType], err = NewBatchNode(nodeCnt, defaultNodePort, utils.MgmtNodeType, "10.211.66", defaultNodeStatus); err != nil {
		return nil, err
	}
	if nodes[utils.OssNodeType], err = NewBatchNode(nodeCnt, defaultNodePort, utils.OssNodeType, "10.211.77", defaultNodeStatus); err != nil {
		return nil, err
	}
	if nodes[utils.CliNodeType], err = NewBatchNode(nodeCnt, defaultNodePort, utils.CliNodeType, "10.145.22", defaultNodeStatus); err != nil {
		return nil, err
	}
	return &FsTopo{
		FsName: fsName,
		Role:   role,
		Nodes:  nodes,
	}, nil
}

func (topo *FsTopo) Dump(file string) error {
	b, err := json.MarshalIndent(topo, " ", " ")
	if err != nil {
		return err
	}
	if err := os.WriteFile(file, b, os.ModePerm); err != nil {
		return err
	}
	return nil
}
