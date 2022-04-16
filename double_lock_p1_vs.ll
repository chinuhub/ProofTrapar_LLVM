; ModuleID = 'double_lock_p1_vs.c'
source_filename = "double_lock_p1_vs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@mutexa = dso_local global i32 0, align 4
@mutexb = dso_local global i32 0, align 4
@count = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %0, %21
  br label %2

2:                                                ; preds = %6, %1
  %3 = cmpxchg i32* @mutexa, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @count, align 4
  %9 = icmp sge i32 %8, -1
  br i1 %9, label %11, label %10

10:                                               ; preds = %7
  call void @__ASSERT()
  br label %11

11:                                               ; preds = %10, %7
  br label %12

12:                                               ; preds = %16, %11
  %13 = cmpxchg i32* @mutexb, i32 0, i32 1 seq_cst seq_cst
  %14 = extractvalue { i32, i1 } %13, 1
  %15 = xor i1 %14, true
  br i1 %15, label %16, label %17

16:                                               ; preds = %12
  br label %12

17:                                               ; preds = %12
  %18 = load i32, i32* @count, align 4
  %19 = icmp eq i32 %18, 0
  br i1 %19, label %21, label %20

20:                                               ; preds = %17
  call void @__ASSERT()
  br label %21

21:                                               ; preds = %20, %17
  store i32 0, i32* @mutexb, align 4
  store i32 0, i32* @mutexa, align 4
  br label %1
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  %1 = alloca i32, align 4
  store i32 3, i32* %1, align 4
  %2 = load i32, i32* %1, align 4
  %3 = icmp eq i32 %2, 3
  br i1 %3, label %4, label %15

4:                                                ; preds = %0
  br label %5

5:                                                ; preds = %9, %4
  %6 = cmpxchg i32* @mutexa, i32 0, i32 1 seq_cst seq_cst
  %7 = extractvalue { i32, i1 } %6, 1
  %8 = xor i1 %7, true
  br i1 %8, label %9, label %10

9:                                                ; preds = %5
  br label %5

10:                                               ; preds = %5
  %11 = load i32, i32* @count, align 4
  %12 = add nsw i32 %11, 1
  store i32 %12, i32* @count, align 4
  %13 = load i32, i32* @count, align 4
  %14 = sub nsw i32 %13, 1
  store i32 %14, i32* @count, align 4
  store i32 0, i32* @mutexa, align 4
  br label %26

15:                                               ; preds = %0
  br label %16

16:                                               ; preds = %20, %15
  %17 = cmpxchg i32* @mutexb, i32 0, i32 1 seq_cst seq_cst
  %18 = extractvalue { i32, i1 } %17, 1
  %19 = xor i1 %18, true
  br i1 %19, label %20, label %21

20:                                               ; preds = %16
  br label %16

21:                                               ; preds = %16
  %22 = load i32, i32* @count, align 4
  %23 = sub nsw i32 %22, 1
  store i32 %23, i32* @count, align 4
  %24 = load i32, i32* @count, align 4
  %25 = add nsw i32 %24, 1
  store i32 %25, i32* @count, align 4
  store i32 0, i32* @mutexb, align 4
  br label %26

26:                                               ; preds = %21, %10
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
