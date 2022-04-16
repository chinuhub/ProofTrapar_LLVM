; ModuleID = 'pthread5_vs.c'
source_filename = "pthread5_vs.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@mutex = dso_local global i32 0, align 4
@g0 = dso_local global i32 0, align 4
@g1 = dso_local global i32 0, align 4
@x = dso_local global i32 0, align 4
@lock = dso_local global i8 0, align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr3() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  store i32 0, i32* @g0, align 4
  store i32 0, i32* @g1, align 4
  store i8 0, i8* @lock, align 1
  store i32 0, i32* @mutex, align 4
  br label %7

7:                                                ; preds = %11, %6
  %8 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %9 = extractvalue { i32, i1 } %8, 1
  %10 = xor i1 %9, true
  br i1 %10, label %11, label %12

11:                                               ; preds = %7
  br label %7

12:                                               ; preds = %7
  %13 = load i8, i8* @lock, align 1
  %14 = trunc i8 %13 to i1
  br i1 %14, label %15, label %23

15:                                               ; preds = %12
  store i32 1, i32* @x, align 4
  %16 = load i32, i32* @g0, align 4
  %17 = icmp eq i32 %16, 1
  br i1 %17, label %18, label %21

18:                                               ; preds = %15
  %19 = load i32, i32* @g1, align 4
  %20 = icmp eq i32 %19, 1
  br i1 %20, label %22, label %21

21:                                               ; preds = %18, %15
  call void @__ASSERT()
  br label %22

22:                                               ; preds = %21, %18
  br label %23

23:                                               ; preds = %22, %12
  store i32 0, i32* @mutex, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %0, %12
  br label %2

2:                                                ; preds = %6, %1
  %3 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %4 = extractvalue { i32, i1 } %3, 1
  %5 = xor i1 %4, true
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @g0, align 4
  %9 = load i32, i32* @g1, align 4
  %10 = icmp eq i32 %8, %9
  br i1 %10, label %12, label %11

11:                                               ; preds = %7
  call void @__ASSERT()
  br label %12

12:                                               ; preds = %11, %7
  store i32 0, i32* @mutex, align 4
  br label %1
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @mutex, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  store i32 1, i32* @g0, align 4
  store i32 1, i32* @g1, align 4
  store i32 0, i32* @mutex, align 4
  store i8 1, i8* @lock, align 1
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
