; ModuleID = 'lu-fig2-fixed.c'
source_filename = "lu-fig2-fixed.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@mThread = dso_local global i32 0, align 4
@start_main = dso_local global i32 0, align 4
@mStartLock = dso_local global i32 0, align 4
@__COUNT__ = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  %1 = alloca i32, align 4
  store i32 1, i32* %1, align 4
  br label %2

2:                                                ; preds = %6, %0
  %3 = cmpxchg i32* @mStartLock, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  store i32 1, i32* @start_main, align 4
  %8 = load i32, i32* @__COUNT__, align 4
  %9 = icmp eq i32 %8, 0
  br i1 %9, label %10, label %14

10:                                               ; preds = %7
  %11 = load i32, i32* %1, align 4
  store i32 %11, i32* @mThread, align 4
  %12 = load i32, i32* @__COUNT__, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, i32* @__COUNT__, align 4
  br label %15

14:                                               ; preds = %7
  call void @__ASSERT()
  br label %15

15:                                               ; preds = %14, %10
  store i32 0, i32* @mStartLock, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  %2 = load i32, i32* @mThread, align 4
  store i32 %2, i32* %1, align 4
  br label %3

3:                                                ; preds = %6, %0
  %4 = load i32, i32* @start_main, align 4
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %3
  br label %3

7:                                                ; preds = %3
  br label %8

8:                                                ; preds = %12, %7
  %9 = cmpxchg i32* @mStartLock, i32 0, i32 1 seq_cst seq_cst
  %10 = extractvalue { i32, i1 } %9, 1
  %11 = xor i1 %10, true
  br i1 %11, label %12, label %13

12:                                               ; preds = %8
  br label %8

13:                                               ; preds = %8
  store i32 0, i32* @mStartLock, align 4
  %14 = load i32, i32* @__COUNT__, align 4
  %15 = icmp eq i32 %14, 1
  br i1 %15, label %16, label %19

16:                                               ; preds = %13
  %17 = load i32, i32* @__COUNT__, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, i32* @__COUNT__, align 4
  br label %20

19:                                               ; preds = %13
  call void @__ASSERT()
  br label %20

20:                                               ; preds = %19, %16
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
