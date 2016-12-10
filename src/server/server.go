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
    var arr []byte
    var path string

    r.ParseForm()

    arr = []byte(r.Form.Get("file"))

    path = string(arr[0]) + "/" + string(arr[1]) + "/" + string(arr[2]) + "/" + string(arr[3])

    _,ok = os.Lstat(path + "/" + r.Form.Get("file"))
    if (nil != ok) {
        http.Error(w, "Not Found", 404)
        return
    }

    file, err := os.Open(path + "/" + r.Form.Get("file"))
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

func upld(w http.ResponseWriter, r *http.Request) {
    var ok error
    var arr []byte
    var path string

    r.ParseForm()

    if (r.Method == "GET") {
        http.Error(w, "Wrong Method", 500)
        return
    } else {
        r.ParseMultipartForm(32 << 20)
        file, handler, err := r.FormFile("uploadfile")
        if err != nil {
            http.Error(w, "111 File write failed", 500)
            fmt.Println(err)
            return
        }

        arr = []byte(handler.Filename)
        path = "./" + string(arr[0]) + "/" + string(arr[1]) + "/" + string(arr[2]) + "/" + string(arr[3]) + "/"

        ok = os.MkdirAll(path, 0700)
        if (nil != ok) {
            http.Error(w, "Directory creation failed", 500)
            return
        }

        defer file.Close()
        fmt.Fprintf(w, "%v", handler.Header)
        f, err := os.OpenFile(path+handler.Filename, os.O_WRONLY|os.O_CREATE, 0666)
        if err != nil {
            fmt.Println(err)
            return
        }
        defer f.Close()
        io.Copy(f, file)
    }
}

func stat(w http.ResponseWriter, r *http.Request) {
    var ok error
    var arr []byte
    var path string

    r.ParseForm()

    arr = []byte(r.Form.Get("file"))

    path = string(arr[0]) + "/" + string(arr[1]) + "/" + string(arr[2]) + "/" + string(arr[3])

    _,ok = os.Lstat(path + "/" + r.Form.Get("file"))
    if (nil == ok) {
        io.WriteString(w, "100 lstat succeeded")
    } else {
        io.WriteString(w, "101 lstat failed")
    }
}

func reg(w http.ResponseWriter, r *http.Request) {
    r.ParseForm()

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

    /* Change directory */
    os.Chdir("/home/backup/SPIDER_BACKUP");

    http.HandleFunc("/get/", get);
    http.HandleFunc("/upld/", upld)
    http.HandleFunc("/stat/", stat)
    http.HandleFunc("/reg/", reg)
    http.ListenAndServe(":8500", nil)
}

