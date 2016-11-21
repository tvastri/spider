package main

import (
        "fmt"
        "os"
        "io"
        "bufio"
        "net/http"
)

func get(w http.ResponseWriter, r *http.Request) {
    var ok error

    r.ParseForm()

    _,ok = os.Lstat(r.Form.Get("file"))
    if (nil != ok) {
        http.Error(w, "Not Found", 404)
        return
    }

    file, err := os.Open(r.Form.Get("file"))
    if (err != nil) {
        http.Error(w, "File open failed", 500)
        return
    }

    w.Header().Set("Content-Type", "application/octet-stream")
    _,err = io.Copy(w, bufio.NewReader(file))
    if (err != nil) {
        http.Error(w, "File write failed", 500)
        return
    }
}

func put(w http.ResponseWriter, r *http.Request) {
    r.ParseForm()
}

func stat(w http.ResponseWriter, r *http.Request) {
    var ok error

    r.ParseForm()
    _,ok = os.Lstat(r.Form.Get("file"))
    if (nil == ok) {
        io.WriteString(w, "200 OK")
    } else {
        http.Error(w, "Not Found", 404)
        return
    }
}

func conf(w http.ResponseWriter, r *http.Request) {
    r.ParseForm()

    fmt.Printf("Hello World\n");

    file, err := os.Open("/etc/spider/server.cfg")
    if (err != nil) {
        http.Error(w, "File open failed", 500)
        return
    }

    w.Header().Set("Content-Type", "application/octet-stream")

    _,err = io.Copy(w, bufio.NewReader(file))
    if (err != nil) {
        http.Error(w, "File write failed", 500)
        return
    }
}

func main() {
        http.HandleFunc("/get/", get);
        http.HandleFunc("/put/", put)
        http.HandleFunc("/stat/", stat)
        http.HandleFunc("/conf/", conf)
        http.ListenAndServe(":8000", nil)
}

