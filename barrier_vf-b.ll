; ModuleID = 'barrier_vf-b.c'
source_filename = "barrier_vf-b.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@MTX = dso_local global i32 0, align 4
@count = dso_local global i32 0, align 4
@COND = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @__ASSERT() #0 {
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr1() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @MTX, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @count, align 4
  %8 = add nsw i32 %7, 1
  store i32 %8, i32* @count, align 4
  %9 = load i32, i32* @count, align 4
  %10 = icmp eq i32 %9, 3
  br i1 %10, label %11, label %12

11:                                               ; preds = %6
  store i32 1, i32* @COND, align 4
  store i32 0, i32* @count, align 4
  br label %19

12:                                               ; preds = %6
  store i32 0, i32* @MTX, align 4
  store i32 0, i32* @COND, align 4
  br label %13

13:                                               ; preds = %17, %12
  %14 = cmpxchg i32* @MTX, i32 0, i32 1 seq_cst seq_cst
  %15 = extractvalue { i32, i1 } %14, 1
  %16 = xor i1 %15, true
  br i1 %16, label %17, label %18

17:                                               ; preds = %13
  br label %13

18:                                               ; preds = %13
  br label %19

19:                                               ; preds = %18, %11
  store i32 0, i32* @MTX, align 4
  call void @__ASSERT()
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @thr2() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %2 = cmpxchg i32* @MTX, i32 0, i32 1 seq_cst seq_cst
  %3 = extractvalue { i32, i1 } %2, 1
  %4 = xor i1 %3, true
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %1

6:                                                ; preds = %1
  %7 = load i32, i32* @count, align 4
  %8 = add nsw i32 %7, 1
  store i32 %8, i32* @count, align 4
  %9 = load i32, i32* @count, align 4
  %10 = icmp eq i32 %9, 3
  br i1 %10, label %11, label %12

11:                                               ; preds = %6
  store i32 1, i32* @COND, align 4
  store i32 0, i32* @count, align 4
  br label %19

12:                                               ; preds = %6
  store i32 0, i32* @MTX, align 4
  store i32 0, i32* @COND, align 4
  br label %13

13:                                               ; preds = %17, %12
  %14 = cmpxchg i32* @MTX, i32 0, i32 1 seq_cst seq_cst
  %15 = extractvalue { i32, i1 } %14, 1
  %16 = xor i1 %15, true
  br i1 %16, label %17, label %18

17:                                               ; preds = %13
  br label %13

18:                                               ; preds = %13
  br label %19

19:                                               ; preds = %18, %11
  store i32 0, i32* @MTX, align 4
  call void @__ASSERT()
  ret void
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 11.0.0-2~ubuntu20.04.1"}
