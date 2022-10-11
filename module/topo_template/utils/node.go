package utils

import (
	"encoding/json"
	"errors"
	"os"

	"github.com/google/uuid"
)

type NodeType struct {
}

const (
	MetaNodeType = iota
	OssNodeType
	MgmtNodeType
	CliNodeType
)

type Node struct {
	Uuid     string `json:"uuid"`
	NodeType string `json:"type"`
	IpAddr   string `json:"ip"`
	Port     int    `json:"port"`
	Status   string `json:"status"`
}

func NewNode(nodeType int, ipAddr string, port int, status string) (*Node, error) {
	if !(nodeType != MetaNodeType || nodeType != OssNodeType || nodeType != MgmtNodeType || nodeType != CliNodeType) {
		return nil, errors.New("invalid Node Type")
	}
	tmpUUid := uuid.New()
	var typeName string
	switch nodeType {
	case MetaNodeType:
		typeName = "meta"
		break
	case OssNodeType:
		typeName = "oss"
		break
	case MgmtNodeType:
		typeName = "mgmt"
		break
	case CliNodeType:
		typeName = "client"
		break

	}
	return &Node{
		Uuid:     tmpUUid.String(),
		NodeType: typeName,
		IpAddr:   ipAddr,
		Port:     port,
		Status:   status,
	}, nil
}

func (n *Node) Load(path string) error {

	var b []byte
	var err error

	if b, err = os.ReadFile(path); err != nil {
		return err
	}
	if err = json.Unmarshal(b, n); err != nil {
		return err
	}
	return nil

}
func (n *Node) Dump(path string) error {
	b, err := json.MarshalIndent(n, " ", " ")
	if err != nil {
		return err
	}

	outFile, err := os.OpenFile(path, os.O_CREATE|os.O_TRUNC|os.O_RDWR, os.ModePerm)
	if err != nil {
		outFile = os.Stdout
	}
	defer outFile.Close()
	if _, err = outFile.Write(b); err != nil {
		return err
	}
	return nil

}
