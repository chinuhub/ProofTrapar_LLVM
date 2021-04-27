; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@state = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %4, %0
  %2 = load i32, i32* @state, align 4
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %1
  br label %1

5:                                                ; preds = %1
  store i32 1, i32* @state, align 4
  %6 = load i32, i32* @state, align 4
  %7 = icmp eq i32 %6, 1
  br i1 %7, label %9, label %8

8:                                                ; preds = %5
  call void @__ASSERT()
  br label %9

9:                                                ; preds = %8, %5
  store i32 0, i32* @state, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  store i32 2, i32* @state, align 4
  %1 = load i32, i32* @state, align 4
  %2 = icmp eq i32 %1, 2
  br i1 %2, label %4, label %3

3:                                                ; preds = %0
  call void @__ASSERT()
  br label %4

4:                                                ; preds = %3, %0
  store i32 0, i32* @state, align 4
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
