package webui

import "testing"

func TestWebui(t *testing.T) {
	want := 0
	if got := webui_test(); got != want {
		t.Errorf("webui_test() = %q, want %q", got, want)
	}
}
